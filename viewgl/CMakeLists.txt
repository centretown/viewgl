project(${CMAKE_PROJECT_NAME})

add_executable(viewgl
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mesh.cpp)

target_include_directories(viewgl
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/cxxopts/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include/glad)

target_link_libraries(viewgl
    PRIVATE
    ${LIBS}
    assimp
    view)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3")
    add_compile_definitions(WEBAPP)
endif()


# if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
#     set_target_properties(viewgl PROPERTIES LINK_FLAGS "-o viewgl.html
#         -s USE_FREETYPE=1 -s DISABLE_EXCEPTION_CATCHING=0 -s DEMANGLE_SUPPORT=1
#         -s SAFE_HEAP=1 --bind -s WASM=1 -O2 -s LEGACY_GL_EMULATION=0
#         -s GL_UNSAFE_OPTS=0 --pre-js pre-module.js --post-js post-module.js
#         -s ASSERTIONS=1 -s GL_ASSERTIONS=1 -s INVOKE_RUN=0  -std=c++17
#         -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s OFFSCREENCANVAS_SUPPORT=1
#         --preload-file ../resources")
# endif()
