
#include "service.h"
#include "service_internal.h"

#include <eoepca/owl/eoepcaows.hpp>
#include <eoepca/owl/owsparameter.hpp>


#include <web/httpfuntions.hpp>
#include <zoo/zooconverter.hpp>

#include <string>
#include <map>

void getT2ConfigurationFromZooMapConfig(maps*& conf, std::string what, std::map<std::string, std::string>& configs) {
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

ZOO_DLL_EXPORT int eoepcaadesdeployprocess(maps*& conf, maps*& inputs, maps*& outputs) {


  std::map<std::string, std::string> confEoepca;
  getT2ConfigurationFromZooMapConfig(conf, "eoepca", confEoepca);


  std::map<std::string, std::string> confMain;
  getT2ConfigurationFromZooMapConfig(conf, "confMain", confMain);

  std::string buffer;
  auto ret =
      getFromWeb(buffer,
                 "https://catalog.terradue.com/eoepca-apps/"
                 "search?format=atom&uid=application_package_sample_app");

  if (ret == 200) {
    setMapInMaps(outputs, "debug", "value", buffer.c_str());
  }else{
    setMapInMaps(outputs, "debug", "value", "no no no nooooooo");
  }







//  setMapInMaps(outputs, "debug", "value", "no no no nooooooo");
  setMapInMaps(outputs, "deployResult", "value", "<CIAO/>");
  return SERVICE_SUCCEEDED;
}



ZOO_DLL_EXPORT int eoepcaadesundeployprocess(maps*& conf, maps*& inputs, maps*& outputs) {



  setMapInMaps(outputs, "unDeployResult", "value", "<CIAO/>");
  return SERVICE_SUCCEEDED;
}



}
