cmake_minimum_required(VERSION 3.5)
project(proc-comm-zoo-svc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH OFF)


find_package(CURL REQUIRED)
INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})


add_subdirectory(3ty/proc-comm-lib-ows)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/3ty/proc-comm-lib-ows/proc-comm-lib-ows/includes)
#include_directories(${PROC_COMM_ZOO_SVC_INCLUDE})

include_directories(includes)


SET(LIB_SRC src/zooconverter.cpp includes/zoo/zooconverter.hpp)


SET(MAIN_SRC
        src/test.cpp
        )


SET(MODS_LIBS
        z m dl
        )


add_library(zooconverter STATIC ${LIB_SRC}  )
TARGET_LINK_LIBRARIES(zooconverter ${CURL_LIBRARIES})

add_executable(xxxx ${MAIN_SRC})
TARGET_LINK_LIBRARIES(xxxx  zooconverter ${MODS_LIBS} ${CURL_LIBRARIES})



