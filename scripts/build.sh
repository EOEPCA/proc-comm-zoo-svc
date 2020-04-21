#!/usr/bin/env bash

docker stop zoo

# enviroments
export DOCKER_ZOO='eoepca/proc-comm-zoo:latest'
export ZOO_BUILD_SERVICE='zoo_build_services'
export ZOO_ZOOSERVICES='zooservices'
export LOCAL_DOCKERIMAGE='eoepca/eoepca-build-cpp:1.0'
export CMAKERELEASE='Release'
export EOEPCA_ZOO='eoepcaadeswps:1.0'

# remove directories
rm -fvR build ${ZOO_BUILD_SERVICE} ${ZOO_ZOOSERVICES}



ls -ltr
echo "Press enter to continue!"
#read

#donload and build
docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${LOCAL_DOCKERIMAGE} cmake -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" ..
docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${LOCAL_DOCKERIMAGE} make eoepcaows  all

echo "Run tests"
./build/test/libtest-test-zooparser


mkdir -p  ${ZOO_BUILD_SERVICE}

#use ZOO Docker
docker pull ${DOCKER_ZOO}

mkdir -p  ${ZOO_ZOOSERVICES}
cp src/zoo/*.zcfg ${ZOO_ZOOSERVICES}/

#docker build --rm -t eoepcaadeswps:1.0 .
docker build --rm -t ${EOEPCA_ZOO} .
docker run -d --rm --name zoo -p 7777:80 -v  $PWD/zooservices:/zooservices ${EOEPCA_ZOO}

docker run  --rm -w /work/${ZOO_BUILD_SERVICE}    -v $PWD:/work ${DOCKER_ZOO} cmake3 -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" -DZOOBUILD=On  ..
docker run  --rm -w /work/${ZOO_BUILD_SERVICE}    -v $PWD:/work ${DOCKER_ZOO} make install

#mv zoo_build_services/libaaaa.so zooservices/eoepcaadesdeployprocess.zo

#curl -s -L  "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=GetCapabilities"
#curl -s -L "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=DescribeProcess&identifier=eoepcaadesdeployprocess"

#curl -s -L "http://localhost:7777/zoo/?service=wps&version=1.0.0&request=Execute&identifier=eoepcaadesdeployprocess&dataInputs=applicationPackage=https%3A%2F%2Fcatalog.terradue.com%2Feoepca-apps%2Fsearch%3Fformat%3Datom%26uid%3Dapplication_package_sample_app;&ResponseDocument=debug@mimeType=text/plain;deployResult@mimeType=application/xml"
#docker run  --rm -w /work/${ZOO_BUILD_SERVICE}    -v $PWD:/work ${DOCKER_ZOO} make install && curl -s -L "http://localhost:7777/zoo/?service=wps&version=1.0.0&request=Execute&identifier=eoepcaadesdeployprocess&dataInputs=applicationPackage=https%3A%2F%2Fcatalog.terradue.com%2Feoepca-apps%2Fsearch%3Fformat%3Datom%26uid%3Dapplication_package_sample_app;&ResponseDocument=debug@mimeType=text/plain;deployResult@mimeType=application/xml"

#curl -s -L "http://localhost:7777/zoo/?service=WPS&version=1.0.0&request=DescribeProcess&identifier=eoepcaadesundeployprocess"
#curl -L  "https://localhost/zoo/?service=wps&version=1.0.0&request=Execute&identifier=TerradueDeployProcess&dataInputs=coordinator=False;&ResponseDocument=deployResult@mimeType=application/xml



