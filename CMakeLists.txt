cmake_minimum_required (VERSION 3.7)
project (mastobotmon
         VERSION 0.2.0
         LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_BINARY_DIR})

# Write version in header
configure_file (
    "${PROJECT_SOURCE_DIR}/src/version.hpp.in"
    "${PROJECT_BINARY_DIR}/version.hpp"
)

file(GLOB sources src/*.cpp)
add_executable(mastobotmon ${sources})
target_link_libraries(mastobotmon mastodon-cpp jsoncpp)
install(TARGETS mastobotmon DESTINATION ${CMAKE_INSTALL_BINDIR})
