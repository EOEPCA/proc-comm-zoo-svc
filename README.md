# proc-comm-zoo-svc
proc-comm-zoo-svc


```shell script
export LOCAL_DOCKERIMAGE='eoepca/eoepca-build-cpp:1.0'
export CMAKERELEASE='Release'

docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${LOCAL_DOCKERIMAGE} cmake -DCMAKE_BUILD_TYPE=${CMAKERELEASE} -G "CodeBlocks - Unix Makefiles" ..

docker run --rm -ti  -v $PWD:/project/ -w /project/build/  ${LOCAL_DOCKERIMAGE} make eoepcaows  all

```
