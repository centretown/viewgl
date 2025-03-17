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
