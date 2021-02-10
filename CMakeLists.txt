### CMake file for gigevision_test
###
###   -  2021: Initial version <seb@homedruon.com>
###
###
###############################################################################


cmake_minimum_required (VERSION 2.6)

project (cam_gigevision)
set (cam_gigevision_VERSION_MAJOR 0)
set (cam_gigevision_VERSION_MINOR 1)
set (cam_gigevision_VERSION_PATCH 1)
set (cam_gigevision_VERSION
	${cam_gigevision_VERSION_MAJOR}.${cam_gigevision_VERSION_MINOR}.${cam_gigevision_VERSION_PATCH} )

set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /lib /usr/lib /usr/local/lib )
set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include )
set( CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  "${PROJECT_SOURCE_DIR}/cmake" ) 

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )
set ( INSTALL_PLUGINS_PATH share/cyan_hwcam )


# Make relative paths absolute (needed later on)

foreach(p LIB BIN HEADERS AR)
    set(var INSTALL_${p}_PATH)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

### Options

option(BUILD_TEST   "Build cyan_gigevision test" ON)


### Search for Lib/Headers

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(libxml2 REQUIRED)
find_package(Threads REQUIRED)
find_package(libzip REQUIRED)
find_package(cyan REQUIRED)
find_package(cyan_hwcam REQUIRED)

### Configure a header file to pass some of the CMake settings
### to the source code

configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_SOURCE_DIR}/src/config_cmake.h"
)


### Set include directories 
        
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/include"
	             "${CMAKE_CURRENT_SOURCE_DIR}/src"
        	     "${CMAKE_CURRENT_BINARY_DIR}"
                 "${CMAKE_INSTALL_PREFIX}/include"
                 "${LIBXML2_INCLUDE_DIRS}" 
)


### File Lists

file( GLOB hdr "src/*.h" )
file( GLOB src "src/*.c" )

### Main Target and linking

add_library( "cam_gigevision" SHARED ${src} ${hdr} )

set_target_properties( cam_gigevision PROPERTIES VERSION ${cam_gigevision_VERSION}
         SOVERSION ${cam_gigevision_VERSION_MAJOR})


target_link_libraries ( "cam_gigevision" m rt
    Threads::Threads
    libzip::zip
    "${CYAN_LIBRARY}"
    "${CYAN_HWCAM_LIBRARY}"
    "${LIBXML2_LIBRARIES}"
)

### Install targets

install( TARGETS "cam_gigevision"
    LIBRARY DESTINATION ${INSTALL_PLUGINS_PATH} )

### tests

if (BUILD_TEST)
    enable_testing()
endif()

### plugins

add_subdirectory(test)

