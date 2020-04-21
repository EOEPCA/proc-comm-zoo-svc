
#include <eoepca/owl/eoepcaows.hpp>
#include <eoepca/owl/owsparameter.hpp>
#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <web/httpfuntions.hpp>
#include <zoo/zooconverter.hpp>

#include "service.h"
#include "service_internal.h"

void getT2ConfigurationFromZooMapConfig(
    maps*& conf, std::string what,
    std::map<std::string, std::string>& configs) {
  maps* t2wps = getMaps(conf, what.c_str());
  if (t2wps && t2wps->content) {
    map* tmp = t2wps->content;
    while (tmp != NULL) {
      configs[tmp->name] = tmp->value;
      tmp = tmp->next;
    }
  }
}

extern "C" {

ZOO_DLL_EXPORT int eoepcaadesdeployprocess(maps*& conf, maps*& inputs,
                                           maps*& outputs) {
  std::map<std::string, std::string> confEoepca;
  getT2ConfigurationFromZooMapConfig(conf, "eoepca", confEoepca);

  std::map<std::string, std::string> confMain;
  getT2ConfigurationFromZooMapConfig(conf, "confMain", confMain);


  map *tmpMap = NULL;

  tmpMap = getMapFromMaps(inputs, "applicationPackage", "value");

//  std::string buffer;
//  auto ret =
//      getFromWeb(buffer,
//                 "https://catalog.terradue.com/eoepca-apps/"
//                 "search?format=atom&uid=application_package_sample_app");
//
//  if (ret == 200) {
//    auto lib = std::make_unique<EOEPCA::EOEPCAows>(confEoepca["owsparser"]);
//
//    if (!lib->IsValid()) {
//      // std::cerr << "can't load libeoepcaows \n";
//
//      setMapInMaps(conf, "lenv", "message", "can't load libeoepcaows");
//      setMapInMaps(conf, "lenv", "code", "noApplicableCode");
//
//      return SERVICE_FAILED;
//    }
//
//    std::unique_ptr<EOEPCA::OWS::OWSContext,
//                    std::function<void(EOEPCA::OWS::OWSContext*)>>
//        ptrContext(lib->parseFromMemory(buffer.c_str(), buffer.size()),
//                   lib->releaseParameter);
//
//    std::stringbuf buffer;
//    std::ostream os(&buffer);
//
//    if (ptrContext) {
//      auto converter = std::make_unique<ZOO::ZooConverter>();
//
//      try {
//        auto out = converter->convert(ptrContext.get());
//        for (auto& single : out) {
//          os << "code: " << single->getCode() << "\n";
//          os << "cwlUri: " << single->getCwlUri() << "\n";
//          os << "dockerRef: " << single->getDockerRef() << "\n";
//
//          for (auto& zoo : single->getZoos()) {
//            os << "Identifier: " << zoo->getIdentifier() << "\n";
//            os << "Config: \n" << zoo->getConfigFile() << "\n";
//          }
//        }
//
//        setMapInMaps(outputs, "debug", "value", buffer.str().c_str());
//
//      } catch (std::runtime_error& err) {
//        setMapInMaps(conf, "lenv", "message", err.what());
//        setMapInMaps(conf, "lenv", "code", "noApplicableCode");
//
//        return SERVICE_FAILED;
//      }
//    }
//
//  } else {
//    setMapInMaps(outputs, "debug", "value", "no no no nooooooo");
//  }

    setMapInMaps(outputs, "debug", "value", tmpMap->value);
  setMapInMaps(outputs, "deployResult", "value", "<CIAO/>");
  return SERVICE_SUCCEEDED;
}

ZOO_DLL_EXPORT int eoepcaadesundeployprocess(maps*& conf, maps*& inputs,
                                             maps*& outputs) {
  setMapInMaps(outputs, "unDeployResult", "value", "<CIAO/>");
  return SERVICE_SUCCEEDED;
}
}
