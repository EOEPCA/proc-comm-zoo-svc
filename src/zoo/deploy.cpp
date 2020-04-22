
#include <cstdio>
#include <eoepca/owl/eoepcaows.hpp>
#include <eoepca/owl/owsparameter.hpp>
#include <fstream>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <utils/xmlmemorywritewrapper.hpp>
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

int removeFile(const char* filename) {
  if (!filename) return 0;
  return std::remove(filename);
}

bool fileExist(const char* fileName) {
  std::ifstream infile(fileName);
  return infile.good();
}

int writeContent(const char* fileName, std::string_view content) {
  std::ofstream file(fileName);
  if (file.good()) {
    file << content;
    file.flush();
    file.close();
  } else {
    return 1;
  }

  return 0;
}

int setZooError(maps* conf, std::string_view message, std::string_view code) {
  setMapInMaps(conf, "lenv", "message", message.data());
  setMapInMaps(conf, "lenv", "code", code.data());

  return SERVICE_FAILED;
}

#define LOGTEST (std::cerr)
void _MEdumpMap(map* t) {
  if (t != NULL) {
    LOGTEST << t->name << ": (" << t->value << ")\n";
    //        if(t->next!=NULL){
    //            // _MEdumpMap(t->next);
    //        }
  } else {
    LOGTEST << "NULL\n";
  }
}

void MEdumpMap(map* t) {
  map* tmp = t;
  while (tmp != NULL) {
    _MEdumpMap(tmp);
    tmp = tmp->next;
  }
  LOGTEST << "----------------END\n";
}

void MEdumpMaps(maps* m) {
  maps* tmp = m;
  while (tmp != NULL) {
    LOGTEST << "\n----------------INI\n";
    LOGTEST << "MAP => [" << tmp->name << "] \n";
    LOGTEST << " * CONTENT [" << tmp->name << "] \n";
    LOGTEST << "----------------VALUE" << std::endl;
    MEdumpMap(tmp->content);
    LOGTEST << " * CHILD [" << tmp->name << "] \n" << std::endl;
    MEdumpMaps(tmp->child);
    tmp = tmp->next;
  }
}

enum class Operation { DEPLOY, UNDEPLOY };
enum class DeployResults { NONE, EXIST, CANTWRITE };

//  std::list<std::unique_ptr<ZOO::Zoo>> zooApplicationOk{};
//  std::list<std::unique_ptr<ZOO::Zoo>> zooApplicationNOk{};

DeployResults deploy(std::string_view path, std::string_view content) {
  if (fileExist(path.data())) {
    return DeployResults::EXIST;
  } else {
    if (writeContent(path.data(), content.data())) {
      return DeployResults::CANTWRITE;
    }
  }

  return DeployResults::NONE;
}

int job(maps*& conf, maps*& inputs, maps*& outputs, Operation operation) {
  std::map<std::string, std::string> confEoepca;
  getT2ConfigurationFromZooMapConfig(conf, "eoepca", confEoepca);

  std::map<std::string, std::string> confMain;
  getT2ConfigurationFromZooMapConfig(conf, "main", confMain);

  if (confMain["servicePath"].empty()) {
    return setZooError(conf, "zoo servicePath empty()", "noApplicableCode");
  }

  if (*confMain["servicePath"].rbegin() != '/') {
    confMain["servicePath"].append("/");
  }

  std::map<std::string, std::string> confMetadata;
  getT2ConfigurationFromZooMapConfig(conf, "metadata", confMetadata);

  map* applicationPackageZooMap =
      getMapFromMaps(inputs, "applicationPackage", "value");

  if (!applicationPackageZooMap) {
    return setZooError(conf, "applicationPackage empty()", "noApplicableCode");
  }

  std::string owsOri(applicationPackageZooMap->value);

  try {
    std::string bufferOWSFile;
    auto ret = getFromWeb(bufferOWSFile, owsOri.c_str());

    if (ret == 200) {
      auto libPath = confEoepca["owsparser"];
      auto lib = std::make_unique<EOEPCA::EOEPCAows>(libPath);
      if (lib->IsValid()) {
        std::list<std::pair<std::string, std::string>> metadata;
        metadata.emplace_back("owsOrigin", std::string(owsOri));

        std::unique_ptr<EOEPCA::OWS::OWSContext,
                        std::function<void(EOEPCA::OWS::OWSContext*)>>
            ptrContext(lib->parseFromMemory(bufferOWSFile.c_str(),
                                            bufferOWSFile.size()),
                       lib->releaseParameter);

        if (ptrContext) {
          auto converter = std::make_unique<ZOO::ZooConverter>();

          std::list<std::string> uniqueTags{};
          uniqueTags.emplace_back(owsOri);

          std::list<std::pair<std::string, std::string>> metadata;
          if (!confMetadata.empty()) {
            for (auto& [k, v] : confMetadata) metadata.emplace_back(k, v);
          }
          metadata.emplace_back("owsOri", owsOri);

          auto out = converter->convert(uniqueTags, ptrContext.get(), metadata);

          std::stringbuf strBuffer;
          auto xml = std::make_unique<XmlWriteMemoryWrapper>();
          xml->startDocument("", "", "");
          xml->startElement("result");
          xml->writeAttribute("applicationPackageFile", owsOri);

          std::string finalPath;
          for (auto& single : out) {
            xml->startElement("operations");
            {
              xml->writeAttribute("packageId", single->getPackageId());

              xml->startElement("operation");
              {
                for (auto& zoo : single->getZoos()) {
                  xml->writeAttribute("processId",
                                      zoo->getProcessDescriptionId());
                  xml->writeAttribute("processVersion",
                                      zoo->getProcessVersion());
                  xml->writeAttribute("version", zoo->getProcessVersion());
                  xml->writeAttribute("type", operation == Operation::DEPLOY
                                                  ? "deploy"
                                                  : "undeploy");

                  finalPath = confMain["servicePath"];
                  finalPath.append(zoo->getIdentifier());

                  xml->startElement("wpsId");
                  { xml->writeContent(zoo->getIdentifier()); }
                  xml->endElement();

                  xml->startElement("title");
                  { xml->writeContent(zoo->getTitle()); }
                  xml->endElement();

                  xml->startElement("abstract");
                  { xml->writeContent(zoo->getAbstract()); }
                  xml->endElement();

                  switch (operation) {
                    case Operation::DEPLOY: {
                      finalPath.append(".zcfg");

                      xml->startElement("status");
                      switch (deploy(finalPath, zoo->getConfigFile())) {
                        case DeployResults::NONE: {
                          xml->writeAttribute("err", "0");
                          xml->writeAttribute("mess", "");
                          xml->writeContent("ready");
                          break;
                        }
                        case DeployResults::CANTWRITE: {
                          xml->writeAttribute("err", "1");
                          xml->writeAttribute(
                              "message", "Can' t write all necessary files");
                          xml->writeContent("not ready");
                          break;
                        }
                        case DeployResults::EXIST: {
                          xml->writeAttribute("err", "2");
                          xml->writeAttribute("message",
                                              "Service already installed");
                          xml->writeContent("not ready");
                          break;
                        }
                      }
                      xml->endElement();
                      break;
                    }
                    case Operation::UNDEPLOY: {
                      finalPath.append(".zcfg");
                      xml->startElement("status");

                      if (fileExist(finalPath.c_str()) &&
                          removeFile(finalPath.c_str())) {
                        xml->writeAttribute("err", "4");
                        xml->writeAttribute("message",
                                            "Can't remove the service");
                        xml->writeContent("removed");
                      } else {
                        xml->writeAttribute("err", "0");
                        xml->writeAttribute("message", "0");
                        xml->writeContent("removed");
                      }

                      xml->endElement();
                      break;
                    }
                  }
                }
              }
              xml->endElement();
            }
            xml->endElement();
          }

          xml->endElement();
          xml->endDocument();

          xml->getBuffer(strBuffer);

          switch (operation) {
            case Operation::DEPLOY: {
              setMapInMaps(outputs, "deployResult", "value",
                           strBuffer.str().c_str());
              break;
            }

            case Operation::UNDEPLOY: {
              setMapInMaps(outputs, "unDeployResult", "value",
                           strBuffer.str().c_str());
              break;
            }
          }

        } else {
          throw std::runtime_error("Error during ows parse!");
        }

      } else {
        std::string err{"Can't load the file: "};
        err.append(libPath);
        throw std::runtime_error(err);
      }

    } else {
      std::string err{"Can't download the file: "};
      err.append(applicationPackageZooMap->value);
      throw std::runtime_error(err);
    }
  } catch (std::runtime_error& err) {
    std::cerr << err.what();
    return setZooError(conf, err.what(), "noApplicableCode");
  } catch (...) {
    std::cerr << "Unexpected server error";
    return setZooError(conf, "Unexpected server error", "noApplicableCode");
  }

  return SERVICE_SUCCEEDED;
}
extern "C" {

ZOO_DLL_EXPORT int eoepcaadesdeployprocess(maps*& conf, maps*& inputs,
                                           maps*& outputs) {
  return job(conf, inputs, outputs, Operation::DEPLOY);
}

ZOO_DLL_EXPORT int eoepcaadesundeployprocess(maps*& conf, maps*& inputs,
                                             maps*& outputs) {
  return job(conf, inputs, outputs, Operation::UNDEPLOY);
}
}
