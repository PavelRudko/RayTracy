if(${PLATFORM} STREQUAL Linux)
    set(BUILD_ZLIB false CACHE BOOL "Build zlib")
    set(BUILD_LIBPNG false CACHE BOOL "Build libpng")
else()
    set(BUILD_ZLIB true CACHE BOOL "Build zlib")
    set(BUILD_LIBPNG true CACHE BOOL "Build libpng")
endif()

if(BUILD_ZLIB)
    include_directories(zlib)
    set(ZLIB_ROOT ${PROJECT_SOURCE_DIR}/Libs/zlib)
    set(ZLIB_INCLUDE_DIR ${CMAKE_BINARY_DIR}/Libs/zlib ${PROJECT_SOURCE_DIR}/Libs/zlib)
    set(ZLIB_LIBRARY ${CMAKE_BINARY_DIR}/bin/zlib)
    add_subdirectory(zlib)
    if(${PLATFORM} STREQUAL Linux)
        set_target_properties(zlibstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set_target_properties (zlibstatic
        PROPERTIES
        FOLDER Libs
    )
endif()

if(BUILD_LIBPNG)
    add_subdirectory(libpng)
    if(${PLATFORM} STREQUAL Linux)
        set_target_properties(pngstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)
    endif()
    set_target_properties (pngstatic genfiles
        PROPERTIES
        FOLDER Libs
    )
endif()

unset(AWK CACHE)
unset(CMAKE_BACKWARDS_COMPATIBILITY CACHE)
unset(CMAKE_INSTALL_PREFIX CACHE)
unset(EXECUTABLE_OUTPUT_PATH CACHE)
unset(LIBRARY_OUTPUT_PATH CACHE)
unset(ld-version-script CACHE)
