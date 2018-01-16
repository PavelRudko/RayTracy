if(BUILD_LIBPNG)
    include_directories(${PROJECT_SOURCE_DIR}/Libs)
    include_directories(${PROJECT_BINARY_DIR}/Libs/libpng)
endif()

add_executable(RayTracy
    TextureLoader.h
    TextureLoader.cpp
    SceneLoader.h
    SceneLoader.cpp
    Geometry.cpp
    Geometry.h
    Renderer.h
    Renderer.cpp
    Texture.h
    Texture.cpp
    Vector.h
    Vector.cpp
    Scene.h
    main.cpp
)

set(DEPENDENCIES "")
set(LIBRARIES "")

if(BUILD_ZLIB)
    set(LIBRARIES $<TARGET_FILE:zlibstatic>)
    set(DEPENDENCIES zlibstatic)
endif()

if(BUILD_LIBPNG)
    set(LIBRARIES $<TARGET_FILE:pngstatic> ${LIBRARIES})
    set(DEPENDENCIES pngstatic ${DEPENDENCIES})
else()
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})
    set(LIBRARIES ${PNG_LIBRARY} ${LIBRARIES})
endif()

target_link_libraries(RayTracy ${LIBRARIES})
if(NOT DEPENDENCIES STREQUAL "")
    add_dependencies(RayTracy ${DEPENDENCIES})
endif()