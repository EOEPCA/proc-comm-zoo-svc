
cmake_minimum_required(VERSION 3.12.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MACOSX_RPATH OFF)

file(GLOB test_sources [a-z_]*.cpp application/[a-z_]*.hpp application/[a-z_]*.cpp)

SET(MODS_LIBS
        z m dl
        )

SET(LIB_TEST_NAME
    libtest-test-zooparser)

SET(SHA1S      ../includes/utils/sha1.hpp
        ../src/sha1.cpp)


add_executable(${LIB_TEST_NAME} ${test_sources} ${SHA1S})
target_link_libraries(${LIB_TEST_NAME} ${MODS_LIBS} gtest_main)
add_test(NAME zooparser_test_run COMMAND ${LIB_TEST_NAME})

