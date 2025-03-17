project(tutgl)

add_library(davegl
    ../glad/src/glad.c
    camera.cpp
    win.cpp
    input_options.cpp
    shader.cpp
    lights.cpp
    texture.cpp)

target_include_directories(davegl
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/cxxopts/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include/glad)
