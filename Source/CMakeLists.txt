cmake_minimum_required(VERSION 2.8)

project(RayTracy)

if(WIN32)
    set(PLATFORM Windows)
    add_definitions(-DPLATFORM_WINDOWS)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER CMake)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
else()
    set(PLATFORM Linux)
    add_definitions(-DPLATFORM_LINUX)
    add_definitions(-std=c++11)
    find_package(X11 REQUIRED)
    link_libraries(${X11_LIBRARIES})
endif()

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

add_subdirectory(RayTracy)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Debug;Release" FORCE)
get_filename_component(ROOT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
set(OUTPUT_BUILD_PREFIX "${ROOT_DIR}/Build" CACHE STRING "${ROOT_DIR}/Build")
set_target_properties(RayTracy PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_BUILD_PREFIX}/${PLATFORM}"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_BUILD_PREFIX}/${PLATFORM}"
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BUILD_PREFIX}/${PLATFORM}"
)