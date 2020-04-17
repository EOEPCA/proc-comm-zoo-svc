

#include "zoo/zooconverter.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <utility>

namespace ZOO {

// if (input->getType() == "BoundingBoxData") {
// auto data =
//    dynamic_cast<EOEPCA::OWS::BoundingBoxData*>(input.get());
// EOEPCA::OWS::Param

template <class T>
std::string parseParam(T* t) {
  return "";
}

void prepareInfos(std::ostream& os, EOEPCA::OWS::Param* param) {
  if (param) {
    os << "\t[" << param->getIdentifier() << "]\n";
    os << "\tTitle = " << param->getTitle() << "\n";
    os << "\tAbstract = " << param->getAbstract() << "\n";

    os << "\tminOccurs =" << param->getMinOccurs() << "\n";
    os << "\tmaxOccurs =" << param->getMaxOccurs() << "\n";
  }
}

template <>
std::string parseParam<EOEPCA::OWS::BoundingBoxData>(
    EOEPCA::OWS::BoundingBoxData* param) {
  if (!param) {
    return "";
  }

  std::stringbuf buffer;
  std::ostream os(&buffer);

  prepareInfos(os, param);

  os << "\t<BoundingBoxData>\n";
  os << "\t</BoundingBoxData>\n";

  return buffer.str();
}

template <>
std::string parseParam<EOEPCA::OWS::LiteralData>(
    EOEPCA::OWS::LiteralData* param) {
  if (!param) {
    return "";
  }

  std::stringbuf buffer;
  std::ostream os(&buffer);

  prepareInfos(os, param);

  os << "\t<LiteralData>\n";
  os << "\t</LiteralData>\n";

  return buffer.str();
}

template <>
std::string parseParam<EOEPCA::OWS::ComplexData>(
    EOEPCA::OWS::ComplexData* param) {
  if (!param) {
    return "";
  }

  std::stringbuf buffer;
  std::ostream os(&buffer);

  prepareInfos(os, param);

  os << "\t<ComplexData>\n";
  os << "\t</ComplexData>\n";

  return buffer.str();
}

template <>
std::string parseParam<EOEPCA::OWS::Param>(EOEPCA::OWS::Param* param) {
  // casting otherwise string check

  auto bb = dynamic_cast<EOEPCA::OWS::BoundingBoxData*>(param);
  if (bb) {
    return parseParam(bb);
  }

  auto ld = dynamic_cast<EOEPCA::OWS::LiteralData*>(param);
  if (ld) {
    return parseParam(ld);
  }

  auto cd = dynamic_cast<EOEPCA::OWS::ComplexData*>(param);
  if (cd) {
    return parseParam(cd);
  }

  return "";
}

class ZooJob {
  std::list<std::pair<std::string, std::string>> metadata{};
  std::string identifier{""}, processVersion{""};
  std::string title{""}, abstract{""};

  std::list<std::string> inputs{};
  std::list<std::string> outputs{};

 public:
  ZooJob() = default;
  virtual ~ZooJob() = default;

 public:
  operator std::string() const;
  friend std::ostream& operator<<(std::ostream& os, const ZooJob& zoo);

  std::string getUniqueService() const;
  void addMetadata(std::string, std::string);

 public:
  void addInput(std::string);
  void addOutput(std::string);

  const std::list<std::string>& getInputs() const;
  const std::list<std::string>& getOutputs() const;

  const std::list<std::pair<std::string, std::string>>& getMetadata() const;
  const std::string& getIdentifier() const;
  void setIdentifier(const std::string& identifier);
  const std::string& getProcessVersion() const;
  void setProcessVersion(const std::string& processVersion);
  const std::string& getTitle() const;
  void setTitle(const std::string& title);
  const std::string& getAbstract() const;
  void setAbstract(const std::string& abstract);
};

void ZooJob::addInput(std::string val) {
  if (!val.empty()) {
    inputs.emplace_back(std::move(val));
  }
}
void ZooJob::addOutput(std::string val) {
  if (!val.empty()) {
    outputs.emplace_back(std::move(val));
  }
}

std::string ZooJob::getUniqueService() const {
  return identifier + "_" + processVersion;
}

ZooJob::operator std::string() const {
  std::string s;
  std::stringbuf buffer;
  std::ostream os(&buffer);
  os << *this;
  s = buffer.str();
  return std::move(s);
}

std::ostream& operator<<(std::ostream& os, const ZooJob& zoo) {
  os << "[" << zoo.getUniqueService() << "]\n";
  os << "Title = " << zoo.title << "\n";
  os << "Abstract = " << zoo.abstract << "\n";
  os << "processVersion = " << zoo.processVersion << "\n";

  os << "storeSupported = true"
     << "\n";
  os << "statusSupported = true"
     << "\n";
  os << "serviceType = C"
     << "\n";
  os << "serviceProvider = " << zoo.getUniqueService() << "\n";

  os << "<MetaData>"
     << "\n";
  for (auto& [key, value] : zoo.getMetadata()) {
    os << "\t" << key << " = " << value << "\n";
  }
  os << "</MetaData>"
     << "\n";

  for (auto& input : zoo.getInputs()) {
    os << "<DataInputs>"
       << "\n";
    os << input << "\n";
    os << "</DataInputs>"
       << "\n";
  }

  for (auto& output : zoo.getOutputs()) {
    os << "<DataOutputs>"
       << "\n";
    os << output << "\n";
    os << "</DataOutputs>"
       << "\n";
  }

  return os;
}
const std::string& ZooJob::getIdentifier() const { return identifier; }
void ZooJob::setIdentifier(const std::string& identifier) {
  ZooJob::identifier = identifier;
}
const std::string& ZooJob::getProcessVersion() const { return processVersion; }
void ZooJob::setProcessVersion(const std::string& processVersion) {
  ZooJob::processVersion = processVersion;
}
const std::string& ZooJob::getTitle() const { return title; }
void ZooJob::setTitle(const std::string& title) { ZooJob::title = title; }
const std::string& ZooJob::getAbstract() const { return abstract; }
void ZooJob::setAbstract(const std::string& abstract) {
  ZooJob::abstract = abstract;
}

void ZooJob::addMetadata(std::string key, std::string value) {
  if (!key.empty() && !value.empty()) {
    metadata.emplace_back(key, value);
  }
}

const std::list<std::pair<std::string, std::string>>& ZooJob::getMetadata()
    const {
  return metadata;
}
const std::list<std::string>& ZooJob::getInputs() const { return inputs; }
const std::list<std::string>& ZooJob::getOutputs() const { return outputs; }

std::list<std::unique_ptr<ZooApplication>> ZooConverter::convert(
    EOEPCA::OWS::OWSContext* owsContext) {
  std::list<std::unique_ptr<ZooApplication>> all;

  for (auto& entry : owsContext->getEntries()) {
    for (auto& offer : entry->getOfferings()) {
      // create application
      auto zooApplication = std::make_unique<ZooApplication>();
      zooApplication->setCode(offer->getCode());

      // contents
      for (auto& content : offer->getContents()) {
        zooApplication->setContent(content.href, content.type);
      }

      // all process
      for (auto& processDescription : offer->getProcessDescription()) {
        auto zoo = std::make_unique<Zoo>();
        auto zooJob = std::make_unique<ZooJob>();

        zooJob->setTitle(processDescription->getTitle());
        zooJob->setAbstract(processDescription->getAbstract());
        zooJob->setIdentifier(processDescription->getIdentifier());
        zooJob->setProcessVersion(processDescription->getVersion());

        auto zooConfig = std::make_unique<Zoo>();
        zooConfig->setIdentifier(std::move(zooJob->getUniqueService()));

        for (auto& input : processDescription->getInputs()) {
          auto res = parseParam<EOEPCA::OWS::Param>(input.get());
          if (!res.empty()) {
            zooJob->addInput(std::move(res));
          }
        }

        for (auto& output : processDescription->getOutputs()) {
          auto res = parseParam<EOEPCA::OWS::Param>(output.get());
          if (!res.empty()) {
            zooJob->addOutput(std::move(res));
          }
        }

        zooConfig->setConfigFile((std::string)*zooJob);

        zooApplication->moveZoo(zooConfig);
      }

      all.emplace_back(std::move(zooApplication));
    }
  }

  return all;
}

const std::string& Zoo::getConfigFile() const { return configFile; }
void Zoo::setConfigFile(const std::string& configFile) {
  Zoo::configFile = configFile;
}
const std::string& Zoo::getIdentifier() const { return identifier; }
void Zoo::setIdentifier(std::string identifier) {
  Zoo::identifier = identifier;
}

void ZooApplication::setContent(std::string_view href, std::string_view type) {
  if (type == "application/vnd.docker.distribution.manifest.v1+json") {
    // GET DOCKER MANIFEST

  } else if (type == "application/cwl") {
    cwlUri = href;
  }
}
void ZooApplication::moveZoo(std::unique_ptr<Zoo>& zoo) {
  if (zoo) {
    Zoos.emplace_back(std::move(zoo));
  }
}

void ZooApplication::setCode(std::string_view theCode) { code = theCode; }
const std::list<std::unique_ptr<Zoo>>& ZooApplication::getZoos() const {
  return Zoos;
}
const std::string& ZooApplication::getCwlUri() const { return cwlUri; }
const std::string& ZooApplication::getDockerRef() const { return dockerRef; }
const std::string& ZooApplication::getCode() const { return code; }

}  // namespace ZOO
