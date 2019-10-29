cmake_minimum_required(VERSION 2.8)

project(arbitrage)


set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_COMPILER "gcc")
set (CMAKE_CXX_COMPILER "g++")

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wconversion -O3 -DNDEBUG -std=c++11")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wconversion -O3 -DNDEBUG -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wconversion -DNDEBUG -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wconversion -DNDEBUG -std=c++11")
set(CMAKE_BUILD_TYPE "debug")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

file(GLOB SOURCE_FILES
    ../src/common/*.cpp
    ../src/boost/*.cpp
    ../src/fix/*.cpp
    ../src/mt4/*.cpp
)
file(GLOB HEADER_FILES
    ../src/common/*.h
    ../src/boost/*.h
    ../src/fix/*.h
    ../src/mt4/*.h
)

find_package (Threads REQUIRED)
#find_package(OpenSSL REQUIRED)

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${BOOST_INCLUDEDIR})
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${BOOST_LIBRARYDIR})

SET(BOOST_USE_STATIC_LIBS        ON)
SET(BOOST_USE_MULTITHREADED      ON)

FIND_PACKAGE(Boost 1.65 COMPONENTS log thread system filesystem REQUIRED)
#find_package(Boost 1.58 COMPONENTS log log_setup system thread REQUIRED)

INCLUDE_DIRECTORIES( ${BOOST_INCLUDEDIR} src include)

add_executable(${PROJECT_NAME} "main.cpp" ${SOURCE_FILES} )

TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
    ${Boost_LIBRARIES}
    quickfix
)

#target_link_libraries(LaserLib
#    ${CMAKE_THREAD_LIBS_INIT}
#    log4cpp
#    gsl
#    gslcblas
    #ssl#
    #MetCore-Commons
    #boost_iostreams
    #boost_system
    #boost_filesystem
#)
