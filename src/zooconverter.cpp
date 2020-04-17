

#include "zoo/zooconverter.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <utility>

namespace ZOO {

class ZooJob {
  std::list<std::pair<std::string, std::string>> metadata{};
  std::string identifier{""}, processVersion{""};
  std::string title{""}, abstract{""};

 public:
  ZooJob() = default;
  virtual ~ZooJob() = default;

 public:
  operator std::string() const;
  friend std::ostream& operator<<(std::ostream& os, const ZooJob& zoo);

  std::string getUniqueService();

  void addMetadata(std::string, std::string);

 public:
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

std::string ZooJob::getUniqueService() {
  std::string val;

  return val;
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
  os << "[" << zoo.identifier << "_" << zoo.processVersion << "]\n";
  os << "Title = " << zoo.title << "\n";
  os << "Abstract = " << zoo.abstract << "\n";
  os << "processVersion = " << zoo.processVersion << "\n";
  os << "<MetaData>" << zoo.processVersion << "\n";
  for (auto& [key, value] : zoo.getMetadata()) {
    os << "\t" << key << " = " << value << "\n";
  }
  os << "</MetaData>" << zoo.processVersion << "\n";

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
        zooConfig->setIdentifier(zooJob->getIdentifier() + "_" +
                                 zooJob->getProcessVersion());

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
void Zoo::setIdentifier(const std::string& identifier) {
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
