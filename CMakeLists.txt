cmake_minimum_required(VERSION 3.5)

project(IntonCore VERSION 0.0.1 DESCRIPTION "IntonCore Library" LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(PROJECT_NAME IntonCore)

set(SPTK_INCLUDE 3party/SPTK-3.11/include)
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

add_library(${PROJECT_NAME} SHARED
    $<TARGET_OBJECTS:modules>
    ${HEADERS}
    ${SOURCES}
    ${SPTK_INCLUDE}/SPTK.h
    ${ALG_SOURCES}
    ${ALG_HEADERS}
)

include_directories(include ${SPTK_INCLUDE} ${ALG_INCLUDE})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

if(ANDROID)
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/../IntonCoreBin/install/lib_android
        ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/../IntonCoreBin/install/lib_android
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/../IntonCoreBin/install/bin_android
    )
else()
    install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/../IntonCoreBin/install/lib
        ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/../IntonCoreBin/install/lib
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/../IntonCoreBin/install/bin
    )
endif()

install(FILES ${HEADERS}
    DESTINATION ${CMAKE_BINARY_DIR}/../IntonCoreBin/install/include)
