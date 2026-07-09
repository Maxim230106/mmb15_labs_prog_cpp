cmake_minimum_required(VERSION 3.16)

set(IP_FILTER_BUILD_NUMBER "$ENV{GITHUB_RUN_NUMBER}")
if (IP_FILTER_BUILD_NUMBER STREQUAL "")
    set(IP_FILTER_BUILD_NUMBER "0")
endif ()

project(ip_filter VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ip_filter ip_filter.cpp)
add_executable(ip_filter_tests tests.cpp)

function(set_project_warnings target)
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else ()
        target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif ()
endfunction()

set_project_warnings(ip_filter)
set_project_warnings(ip_filter_tests)

include(CTest)
add_test(NAME ip_filter_tests COMMAND $<TARGET_FILE:ip_filter_tests>)

install(TARGETS ip_filter RUNTIME DESTINATION bin)

if (WIN32)
    set(CPACK_GENERATOR "ZIP")
else ()
    set(CPACK_GENERATOR "TGZ")
endif ()

string(TOLOWER "${CMAKE_SYSTEM_NAME}" CPACK_SYSTEM_NAME_LOWER)
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}.${IP_FILTER_BUILD_NUMBER}")
set(CPACK_PACKAGE_FILE_NAME "ip_filter-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME_LOWER}")
set(CPACK_PACKAGE_CONTACT maxim230106@gmail.com)

include(CPack)
