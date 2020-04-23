#!/usr/bin/env bash

#docker stop zoo

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


docker pull ${LOCAL_DOCKERIMAGE}
if [ $? -ne 0 ]
then
  echo "Can't pull the  Docker Image: ${LOCAL_DOCKERIMAGE}"
  exit 1
fi



docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${LOCAL_DOCKERIMAGE} cmake -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" ..
if [ $? -ne 0 ]
then
  echo "CMAKE release ${CMAKERELEASE} failed"
  exit 2
fi

docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${LOCAL_DOCKERIMAGE} make eoepcaows  all
if [ $? -ne 0 ]
then
  echo "Make failed"
  exit 2
fi

echo "Run tests"
./build/test/libtest-test-zooparser
if [ $? -ne 0 ]
then
  echo "tests failed"
  exit 3
fi

mkdir -p  ${ZOO_BUILD_SERVICE}
if [ $? -ne 0 ]
then
  echo "Can't prepare the folder ${ZOO_BUILD_SERVICE}"
  exit 4
fi

#use ZOO Docker
docker pull ${DOCKER_ZOO}
if [ $? -ne 0 ]
then
  echo "Can't pull the  Docker image: ${DOCKER_ZOO}"
  exit 5
fi


mkdir -p  ${ZOO_ZOOSERVICES}
if [ $? -ne 0 ]
then
  echo "Can't prepare the folder ${ZOO_ZOOSERVICES}"
  exit 4
fi

cp src/zoo/*.zcfg ${ZOO_ZOOSERVICES}/
if [ $? -ne 0 ]
then
  echo "Can't prepare the zoo configuration file"
  exit 4
fi

docker run  --rm -w /work/${ZOO_BUILD_SERVICE}  -v $PWD:/work ${DOCKER_ZOO} cmake3 -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" -DZOOBUILD=On  ..
if [ $? -ne 0 ]
then
  echo "CMAKE release ${CMAKERELEASE} failed"
  exit 2
fi


docker run  --rm -w /work/${ZOO_BUILD_SERVICE}  -v $PWD:/work ${DOCKER_ZOO} make
if [ $? -ne 0 ]
then
  echo "Make failed"
  exit 2
fi


docker build --rm -t ${EOEPCA_ZOO} .
if [ $? -ne 0 ]
then
  echo "Build ${EOEPCA_ZOO} failed"
  exit 2
fi


