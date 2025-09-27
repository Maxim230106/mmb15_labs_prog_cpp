cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "0" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(helloworld VERSION ${PROJECT_VERSION} LANGUAGES CXX)
add_executable(helloworld main.cpp)
target_compile_definitions(helloworld PRIVATE APP_VERSION="${PROJECT_VERSION}")

set(CPACK_GENERATOR DEB)
