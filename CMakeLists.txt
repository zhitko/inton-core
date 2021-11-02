cmake_minimum_required(VERSION 3.5)

project(IntonCore VERSION 0.0.1 DESCRIPTION "IntonCore Library" LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(PROJECT_NAME IntonCore)

set(ALG_INCLUDE 3party/alglib-3.16.0/src)

file(GLOB MODULES_SOURCES modules/*/*.cpp)
file(GLOB MODULES_HEADERS modules/*/*.h)

add_library(modules OBJECT
    ${MODULES_SOURCES}
    ${MODULES_HEADERS}
)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)

file(GLOB ALG_SOURCES ${ALG_INCLUDE}/*.cpp)
file(GLOB ALG_HEADERS ${ALG_INCLUDE}/*.h)

set(SPTK 3party/SPTK/)
file(GLOB SPTK_SOURCES
    ${SPTK}/src/*/*.cc
    ${SPTK}/third_party/*/*/*.cc
)
list(FILTER SPTK_SOURCES EXCLUDE REGEX ".*/main/.*")
file(GLOB SPTK_HEADERS
    ${SPTK}/include/SPTK/*/*.h
    ${SPTK}/third_party/*/*/*.h
)

add_library(${PROJECT_NAME} SHARED
    $<TARGET_OBJECTS:modules>
    ${HEADERS}
    ${SOURCES}
    ${SPTK_SOURCES}
    ${SPTK_HEADERS}
    ${ALG_SOURCES}
    ${ALG_HEADERS}
)

include_directories(include
    ${SPTK}/include
    ${SPTK}/third_party
    ${SPTK}/third_party/REAPER
    ${SPTK}/third_party/WORLD/src
    ${ALG_INCLUDE}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)
