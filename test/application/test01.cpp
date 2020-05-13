

#include <sys/stat.h>

#include <eoepca/owl/eoepcaows.hpp>
#include <eoepca/owl/owsparameter.hpp>
#include <web/httpfuntions.hpp>
#include <zoo/zooconverter.hpp>

#include "gtest/gtest.h"

std::string getBasePat() {
  //  const std::string &s

  struct stat buffer;
  if (stat("./cmake-build-debug", &buffer) == 0) {
    return "./cmake-build-debug";
  }

  return "./build";
}

TEST(zooparser, 01_getFileFromInternet) {
  std::string buffer;
  auto ret =
      getFromWeb(buffer,
                 "https://catalog.terradue.com/eoepca-apps/"
                 "search?format=atom&uid=application_package_sample_app");
  std::cout << buffer << "\n";
  EXPECT_EQ(ret, 200);
}

TEST(zooparser, 02_loadOwsLib) {
  auto libMac = std::make_unique<EOEPCA::EOEPCAows>(
      getBasePat() +
      "/3ty/proc-comm-lib-ows-1.0/"
      "libeoepcaows.dylib");

  auto libLinux = std::make_unique<EOEPCA::EOEPCAows>(
      getBasePat() +
      "/3ty/proc-comm-lib-ows-1.0/"
      "libeoepcaows.so");

  EXPECT_TRUE(libMac->IsValid() || libLinux->IsValid());
}

TEST(zooparser, 03_createContext) {
  std::string buffer;
  auto ret =
      getFromWeb(buffer,
                 "https://catalog.terradue.com/eoepca-apps/"
                 "search?format=atom&uid=application_package_sample_app");

  EXPECT_EQ(ret, 200);

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(
      getBasePat() +
      "/3ty/proc-comm-lib-ows-1.0/libeoepcaows.dylib");

  if (!lib->IsValid()) {
    lib = std::make_unique<EOEPCA::EOEPCAows>(
        getBasePat() +
        "/3ty/proc-comm-lib-ows-1.0/libeoepcaows.so");
  }

  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext *)>>
      ptrContext(lib->parseFromMemory(buffer.c_str(), buffer.size()),
                 lib->releaseParameter);

  EXPECT_TRUE(ptrContext);
}

TEST(zooparser, 04_createContext) {
  std::string buffer;
  auto ret =
      getFromWeb(buffer,
                 "https://catalog.terradue.com/eoepca-apps/"
                 "search?format=atom&uid=application_package_sample_app");

  EXPECT_EQ(ret, 200);

  auto lib = std::make_unique<EOEPCA::EOEPCAows>(
      getBasePat() +
      "/3ty/proc-comm-lib-ows-1.0/"
      "libeoepcaows.dylib");

  if (!lib->IsValid()) {
    lib = std::make_unique<EOEPCA::EOEPCAows>(
        getBasePat() +
        "/3ty/proc-comm-lib-ows-1.0/"
        "libeoepcaows.so");
  }

  std::unique_ptr<EOEPCA::OWS::OWSContext,
                  std::function<void(EOEPCA::OWS::OWSContext *)>>
      ptrContext(lib->parseFromMemory(buffer.c_str(), buffer.size()),
                 lib->releaseParameter);

  EXPECT_TRUE(ptrContext);

  std::list<std::string> uniqueTags{};

  auto converter = std::make_unique<ZOO::ZooConverter>();
  auto out = converter->convert(uniqueTags, ptrContext.get());

  for (auto &single : out) {
    EXPECT_EQ(
        "https://store.terradue.com/api/eoepca/_applications/eoepca/sample-app/"
        "0.0.1.2/sample-app.0.0.1.2-application.cwl",
        single->getCwlUri());

    for (auto &zoo : single->getZoos()) {
      std::cout << "Identifier: " << zoo->getIdentifier() << "\n";
      EXPECT_EQ("w300ab26fb930b65dbe1c886745ea827d284d90f3",
                zoo->getIdentifier());

      std::cout << "Provider: " << zoo->getProvider() << "\n";
      EXPECT_EQ("w300ab26fb930b65dbe1c886745ea827d284d90f3.zo",
                zoo->getProvider());

      std::cout << "title: " << zoo->getTitle() << "\n";
      EXPECT_EQ("A sample App", zoo->getTitle());

      std::cout << "abstract: " << zoo->getAbstract() << "\n";
      EXPECT_EQ("Sample abstract", zoo->getAbstract());
    }
  }

  EXPECT_TRUE(true);
}
