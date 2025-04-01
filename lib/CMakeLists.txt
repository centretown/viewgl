project(${CMAKE_PROJECT_NAME})

# set(CMAKE_CXX_FLAGS -DIMGUI_IMPL_OPENGL_ES2)

add_library(view

    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/backends/imgui_impl_opengl3.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/../src/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/win.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/options.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/lights.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/texture.cpp)

target_include_directories(view
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/assimpjs/assimp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/cxxopts/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include/glad)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sUSE_GLFW=3 --use-port=contrib.glfw3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_GLFW=3 --use-port=contrib.glfw3")
    add_compile_definitions(WEBAPP)
endif()
