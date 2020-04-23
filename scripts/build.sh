#!/usr/bin/env bash


# export 
export ZOO_BUILD_SERVICE='zoo_build_services'
export ZOO_ZOOSERVICES='zooservices'

if [ -z "${DOCKER_ZOO}" ]
then
  DOCKER_ZOO='eoepca/proc-comm-zoo:latest'
fi

if [ -z "${BUILDER_DOCKERIMAGE}" ]
then
  BUILDER_DOCKERIMAGE='eoepca/eoepca-build-cpp:1.0'
fi

if [ -z "${CMAKERELEASE}" ]
then
  CMAKERELEASE='Release'
fi

if [ -z "${LOCAL_IMAGE_NAME}" ]
then
  LOCAL_IMAGE_NAME='eoepca-ades-wps:1.0'
fi

# remove directories
rm -fvR build ${ZOO_BUILD_SERVICE} ${ZOO_ZOOSERVICES}

ls -ltr

docker pull ${BUILDER_DOCKERIMAGE}
if [ $? -ne 0 ]
then
  echo "Can't pull the Docker Image: ${BUILDER_DOCKERIMAGE}"
  exit 1
fi

docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${BUILDER_DOCKERIMAGE} cmake -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" ..
if [ $? -ne 0 ]
then
  echo "CMAKE release ${CMAKERELEASE} failed"
  exit 2
fi

docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${BUILDER_DOCKERIMAGE} make eoepcaows  all
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


docker build --rm -t ${LOCAL_IMAGE_NAME} .
if [ $? -ne 0 ]
then
  echo "Build ${LOCAL_IMAGE_NAME} failed"
  exit 2
fi


