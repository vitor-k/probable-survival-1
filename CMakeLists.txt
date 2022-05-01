cmake_minimum_required (VERSION 3.8)

project(PS1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set up output paths for executable binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/$<CONFIG>)

option(ENABLE_TESTS "Build tests" OFF)

if(ENABLE_TESTS)
    enable_testing()
endif()

if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ProSur1)
endif()

add_subdirectory(externals)
add_subdirectory(src)
