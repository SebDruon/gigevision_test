### CMake file for gigevision_test
###
###   -  2021: Initial version <seb@homedruon.com>
###
###
###############################################################################


cmake_minimum_required (VERSION 2.6)

project (gev_test)
set (gev_test_VERSION_MAJOR 0)
set (gev_test_VERSION_MINOR 1)

set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /lib /usr/lib /usr/local/lib )
set( CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /usr/include /usr/local/include )
set( CMAKE_MODULE_PATH  ${CMAKE_MODULE_PATH}  "${PROJECT_SOURCE_DIR}/cmake" ) 

set ( INSTALL_BIN_PATH bin )
set ( INSTALL_LIB_PATH lib )
set ( INSTALL_AR_PATH  lib )
set ( INSTALL_HEADERS_PATH include )


# Make relative paths absolute (needed later on)

foreach(p LIB BIN HEADERS AR)
    set(var INSTALL_${p}_PATH)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()


### Search for Lib/Headers

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(cyan REQUIRED)

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
)


### File Lists

file( GLOB hdr "src/*.h*" )
file( GLOB src "src/*.c" )

### Main Target and linking

add_executable( "gev_test" ${src} ${hdr} )

target_link_libraries ( "gev_test" m rt
    Threads::Threads
    "${CYAN_LIBRARY}"
)

### Install targets

install( TARGETS "gev_test"
        RUNTIME DESTINATION ${INSTALL_BIN_PATH}  COMPONENT main
        LIBRARY DESTINATION ${INSTALL_LIB_PATH}  COMPONENT main
        ARCHIVE DESTINATION ${INSTALL_AR_PATH}   COMPONENT main )
