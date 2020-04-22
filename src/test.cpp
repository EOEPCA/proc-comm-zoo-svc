

#include <eoepca/owl/eoepcaows.hpp>
#include <eoepca/owl/owsparameter.hpp>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <web/httpfuntions.hpp>
#include <zoo/zooconverter.hpp>

int main(int argc, const char **argv) {
  try {
    std::string buffer;
    auto ret =
        getFromWeb(buffer,
                   "https://catalog.terradue.com/eoepca-apps/"
                   "search?format=atom&uid=application_package_sample_app");

    if (ret == 200) {
      auto lib = std::make_unique<EOEPCA::EOEPCAows>(
          "/Users/rdirienzo/Project/EOEPCA/proc-comm-zoo-svc/cmake-build-debug/"
          "3ty/"
          "proc-comm-lib-ows/proc-comm-lib-ows-build/libeoepcaows.dylib");
      if (!lib->IsValid()) {
        // build mac
        lib = std::make_unique<EOEPCA::EOEPCAows>(
            "/Users/rdirienzo/Project/EOEPCA/proc-comm-zoo-svc/"
            "cmake-build-debug/"
            "3ty/proc-comm-lib-ows/proc-comm-lib-ows-build/libeoepcaows.dylib");
      }

      if (!lib->IsValid()) {
        std::cerr << "can't load libeoepcaows \n";

        return 199;
      }

      std::unique_ptr<EOEPCA::OWS::OWSContext,
                      std::function<void(EOEPCA::OWS::OWSContext *)>>
          ptrContext(lib->parseFromMemory(buffer.c_str(), buffer.size()),
                     lib->releaseParameter);

      if (ptrContext) {

        std::list<std::string> uniqueTags{};
        uniqueTags.emplace_back("cacsa");
        uniqueTags.emplace_back("cacs2a");
        uniqueTags.emplace_back("cacs4a");

        auto converter = std::make_unique<ZOO::ZooConverter>();
        auto out = converter->convert(uniqueTags,ptrContext.get());

        for (auto &single : out) {
          std::cout << "code: " << single->getCode() << "\n";
          std::cout << "cwlUri: " << single->getCwlUri() << "\n";
          std::cout << "dockerRef: " << single->getDockerRef() << "\n";

          for (auto &zoo : single->getZoos()) {

            std::cout << "Identifier: " << zoo->getIdentifier() << "\n";
            std::cout << "Provider: " << zoo->getProvider() << "\n";
//            std::cout << "Config: \n"<< zoo->getConfigFile() << "\n";

            std::cout << "title: " << zoo->getTitle() << "\n";
            std::cout << "abstract: " << zoo->getAbstract() << "\n";


          }
        }
      }
    } else {
      std::cerr << "can't download file!\n";
    }
  } catch (std::runtime_error &e) {
    std::cerr << e.what() << "\n";
  }

  return 0;
}
