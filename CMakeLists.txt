cmake_minimum_required(VERSION 3.5)

project(CppUtilities LANGUAGES CXX)

option(TESTING "Build tests" ON)

set(CPPUTIL_TARGET_NAME ${PROJECT_NAME})
set(CPPUTIL_INCLUDE_DIRECTORY "include/")
set(CPPUTIL_INCLUDE_DESTINATION "$ENV{HOME}/.local/include/cpp/")

add_library(${CPPUTIL_TARGET_NAME} INTERFACE)
target_include_directories(${CPPUTIL_TARGET_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CPPUTIL_INCLUDE_DESTINATION}>
)

install(DIRECTORY ${CPPUTIL_INCLUDE_DIRECTORY} DESTINATION ${CPPUTIL_INCLUDE_DESTINATION})

if(TESTING)
    enable_testing(true)
    add_subdirectory(test/3rdparty/Catch)
    add_subdirectory(test)
endif()
