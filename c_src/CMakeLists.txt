
cmake_minimum_required(VERSION 3.20)

project(q330)

set(LIB330_SRC_DIR ${PROJECT_BINARY_DIR}/earthworm-prefix/src/earthworm/src/libsrc/lib330)
set(LIBMSEED_SRC_DIR ${PROJECT_BINARY_DIR}/earthworm-prefix/src/earthworm/src/libsrc/util/libmseed)
set(Q330_SRC_DIR ${PROJECT_SOURCE_DIR}/include/lsst/ts/ess/earthquake/python)

include(ExternalProject)
ExternalProject_Add(earthworm
    GIT_REPOSITORY https://gitlab.com/seismic-software/earthworm.git
    GIT_TAG master
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ../../../../build_earthworm.sh
    BUILD_ALWAYS true
    INSTALL_COMMAND ""
    LOG_BUILD 1
)

add_library(q330 SHARED ${Q330_SRC_DIR}/q330.c)
add_dependencies(q330 earthworm)
target_link_libraries(q330 ${LIB330_SRC_DIR}/lib330.a ${LIBMSEED_SRC_DIR}/libmseed.a pthread)
target_include_directories(q330 PUBLIC ${Q330_SRC_DIR} ${LIB330_SRC_DIR})
target_link_directories(q330 PUBLIC ${Q330_SRC_DIR} ${LIBMSEED_SRC_DIR} ${LIB330_SRC_DIR})
install(TARGETS q330 DESTINATION ${PROJECT_SOURCE_DIR}/../python/lsst/ts/ess/earthquake/data/lib330)

add_executable(connect_q330 ${PROJECT_SOURCE_DIR}/connect_q330.c)
add_dependencies(connect_q330 q330 earthworm)
target_include_directories(connect_q330 PUBLIC ${Q330_SRC_DIR} ${LIBMSEED_SRC_DIR} ${LIB330_SRC_DIR})
target_link_libraries(connect_q330 q330 m)
