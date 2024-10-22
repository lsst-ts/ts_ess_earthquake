
cmake_minimum_required(VERSION 3.20)

project(q330)

IF(APPLE)
    SET(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)
ENDIF(APPLE)

set(LIB330_SRC_DIR ${PROJECT_SOURCE_DIR}/include/lib330)
set(LIBMSEED_SRC_DIR ${PROJECT_SOURCE_DIR}/include/libmseed)
set(Q330_SRC_DIR ${PROJECT_SOURCE_DIR}/include/lsst/ts/ess/earthquake/python)

add_library(q330 SHARED ${Q330_SRC_DIR}/q330.c
        ${LIB330_SRC_DIR}/libarchive.c
        ${LIB330_SRC_DIR}/libclient.c
        ${LIB330_SRC_DIR}/libcmds.c
        ${LIB330_SRC_DIR}/libcompress.c
        ${LIB330_SRC_DIR}/libcont.c
        ${LIB330_SRC_DIR}/libctrldet.c
        ${LIB330_SRC_DIR}/libcvrt.c
        ${LIB330_SRC_DIR}/libdetect.c
        ${LIB330_SRC_DIR}/libdss.c
        ${LIB330_SRC_DIR}/libfilters.c
        ${LIB330_SRC_DIR}/liblogs.c
        ${LIB330_SRC_DIR}/libmd5.c
        ${LIB330_SRC_DIR}/libmsgs.c
        ${LIB330_SRC_DIR}/libnetserv.c
        ${LIB330_SRC_DIR}/libopaque.c
        ${LIB330_SRC_DIR}/libpoc.c
        ${LIB330_SRC_DIR}/libsampcfg.c
        ${LIB330_SRC_DIR}/libsample.c
        ${LIB330_SRC_DIR}/libseed.c
        ${LIB330_SRC_DIR}/libslider.c
        ${LIB330_SRC_DIR}/libstats.c
        ${LIB330_SRC_DIR}/libstrucs.c
        ${LIB330_SRC_DIR}/libsupport.c
        ${LIB330_SRC_DIR}/libtokens.c
        ${LIB330_SRC_DIR}/libtypes.c
        ${LIB330_SRC_DIR}/libverbose.c
        ${LIB330_SRC_DIR}/q330cvrt.c
        ${LIB330_SRC_DIR}/q330io.c
        ${LIBMSEED_SRC_DIR}/fileutils.c
        ${LIBMSEED_SRC_DIR}/genutils.c
        ${LIBMSEED_SRC_DIR}/gswap.c
        ${LIBMSEED_SRC_DIR}/lmplatform.c
        ${LIBMSEED_SRC_DIR}/logging.c
        ${LIBMSEED_SRC_DIR}/lookup.c
        ${LIBMSEED_SRC_DIR}/msrutils.c
        ${LIBMSEED_SRC_DIR}/pack.c
        ${LIBMSEED_SRC_DIR}/packdata.c
        ${LIBMSEED_SRC_DIR}/parseutils.c
        ${LIBMSEED_SRC_DIR}/selection.c
        ${LIBMSEED_SRC_DIR}/tracelist.c
        ${LIBMSEED_SRC_DIR}/traceutils.c
        ${LIBMSEED_SRC_DIR}/unpack.c
        ${LIBMSEED_SRC_DIR}/unpackdata.c
)
target_link_libraries(q330 pthread)
target_include_directories(q330 PUBLIC ${Q330_SRC_DIR} ${LIB330_SRC_DIR} ${LIBMSEED_SRC_DIR})
target_link_directories(q330 PUBLIC ${Q330_SRC_DIR} ${LIB330_SRC_DIR} ${LIBMSEED_SRC_DIR})
install(TARGETS q330 DESTINATION ${PROJECT_SOURCE_DIR}/../python/lsst/ts/ess/earthquake/data/libq330)

add_executable(connect_q330 ${PROJECT_SOURCE_DIR}/connect_q330.c)
add_dependencies(connect_q330 q330)
target_include_directories(connect_q330 PUBLIC ${Q330_SRC_DIR} ${LIBMSEED_SRC_DIR} ${LIB330_SRC_DIR})
target_link_libraries(connect_q330 q330 m)
