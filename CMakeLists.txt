cmake_minimum_required(VERSION 3.0.0)
project(fake-data-generator VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(fake-data-generator main.cpp ${SOURCE_FILES} ${HEADER_FILES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})