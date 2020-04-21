
if (ZOOBUILD)

    cmake_minimum_required(VERSION 2.8)
    project(zooServiceBuild)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_MACOSX_RPATH OFF)
    set(CMAKE_CXX_FLAGS "-fPIC")

    #XML dependences--------------------------------------

    find_package(CURL REQUIRED)
    include_directories(${CURL_INCLUDE_DIRS})

    find_package(LibXml2 REQUIRED)
    find_package(LibXslt REQUIRED)
    include_directories(${LIBXML2_INCLUDE_DIR})
    include_directories(${LIBXSLT_INCLUDE_DIR})
    #--------------------------------------------------------


    #${LIBXML2_LIBRARIES}

    include_directories(includes)
    include_directories(/var/lib64 /usr/local/include/t2wpsZoolib /opt/t2build/includes/ /usr/local/include/zoo . ./include
            )

    include_directories(build/3ty/proc-comm-lib-ows/proc-comm-lib-ows/includes)
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/3ty/proc-comm-lib-ows/proc-comm-lib-ows/includes)


    SET(LIB_SRC
            src/zoo/deploy.cpp
            )

    SET(MODS_LIBS
            z m dl
            )


    add_library(epcatransactional SHARED ${LIB_SRC})
    TARGET_LINK_LIBRARIES(epcatransactional ${CURL_LIBRARIES} zoo_service ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} ${MODS_LIBS} ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES})


    install(TARGETS epcatransactional DESTINATION /work/zooservices/)

else ()

    cmake_minimum_required(VERSION 3.5)
    project(proc-comm-zoo-svc)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_MACOSX_RPATH OFF)


    find_package(CURL REQUIRED)
    include_directories(${CURL_INCLUDE_DIRS})

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

    add_library(zooconverter STATIC ${LIB_SRC})
    TARGET_LINK_LIBRARIES(zooconverter ${CURL_LIBRARIES})

    add_executable(xxxx ${MAIN_SRC})
    TARGET_LINK_LIBRARIES(xxxx zooconverter ${MODS_LIBS} ${CURL_LIBRARIES})


endif ()


#set(var1 OFF)
#set(var2 "var1")
#if(${var2})


