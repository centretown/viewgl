project(${CMAKE_PROJECT_NAME})

add_library(imguigl
    ../external/imgui/imgui.cpp
    ../external/imgui/imgui_draw.cpp
    ../external/imgui/imgui_demo.cpp
    ../external/imgui/imgui_tables.cpp
    ../external/imgui/imgui_widgets.cpp
    ../external/imgui/backends/imgui_impl_glfw.cpp
    ../external/imgui/backends/imgui_impl_opengl3.cpp)

target_include_directories(imguigl
    PUBLIC ../external/imgui/ ../external/imgui/backends)

add_library(davegl
    ../glad/src/glad.c
    ../src/camera.cpp
    ../src/win.cpp
    ../src/options.cpp
    ../src/shader.cpp
    ../src/lights.cpp
    ../src/texture.cpp)

target_include_directories(davegl
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/cxxopts/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include/glad)
