
#ifndef PROC_COMM_ZOO_SVC_ZOOCONVERTER_HPP
#define PROC_COMM_ZOO_SVC_ZOOCONVERTER_HPP

#include <eoepca/owl/owsparameter.hpp>
#include <list>
#include <memory>
#include <string>

namespace ZOO {

class Zoo {
  std::string configFile{""};
  std::string identifier{""};

 public:
  Zoo() = default;
  virtual ~Zoo() = default;

  const std::string &getConfigFile() const;
  void setConfigFile(const std::string &configFile);

  const std::string &getIdentifier() const;
  void setIdentifier(std::string identifier);
};

class ZooApplication {
  std::list<std::unique_ptr<Zoo>> Zoos;
  std::string cwlUri{""};
  std::string dockerRef{""};
  std::string code{""};

 public:
  ZooApplication() = default;
  virtual ~ZooApplication() = default;

  void setCode(std::string_view code);
  void setContent(std::string_view href, std::string_view type );

  void moveZoo( std::unique_ptr<Zoo>& zoo );

  const std::list<std::unique_ptr<Zoo>> &getZoos() const;
  const std::string &getCwlUri() const;
  const std::string &getDockerRef() const;
  const std::string &getCode() const;
};

class ZooConverter {
 public:
  ZooConverter() = default;
  virtual ~ZooConverter() = default;

  std::list<std::unique_ptr<ZooApplication>> convert(EOEPCA::OWS::OWSContext *);
};

}  // namespace ZOO

#endif  // PROC_COMM_ZOO_SVC_ZOOCONVERTER_HPP
