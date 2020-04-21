
#include "service.h"
#include "service_internal.h"

//#include <eoepca/owl/eoepcaows.hpp>
//#include <eoepca/owl/owsparameter.hpp>



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


  setMapInMaps(outputs, "debug", "value", confEoepca["owsparser"].c_str());
  setMapInMaps(outputs, "deployResult", "value", "<CIAO/>");
  return SERVICE_SUCCEEDED;
}



ZOO_DLL_EXPORT int eoepcaadesundeployprocess(maps*& conf, maps*& inputs, maps*& outputs) {



  setMapInMaps(outputs, "unDeployResult", "value", "<CIAO/>");
  return SERVICE_SUCCEEDED;
}



}
