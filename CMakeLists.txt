cmake_minimum_required(VERSION 3.5)
project(proc-comm-zoo-svc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH OFF)

add_subdirectory(3ty/proc-comm-lib-ows)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/3ty/proc-comm-lib-ows/proc-comm-lib-ows/includes)

include_directories(includes)

SET(MAIN_SRC
        src/test.cpp
        )


add_executable(xxxx ${MAIN_SRC})
TARGET_LINK_LIBRARIES(xxxx)



