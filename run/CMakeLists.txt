project(${CMAKE_PROJECT_NAME})

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()


add_executable(viewgl
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mesh.cpp)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -sUSE_GLFW=3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_GLFW=3")
    # add_link_options("-sFULL_ES3 -sINITIAL_HEAP=1024MB -sMAX_WEBGL_VERSION=2")
    add_compile_definitions(WEBAPP E)
    set_target_properties(viewgl PROPERTIES LINK_FLAGS
        "-sASYNCIFY --use-port=contrib.glfw3 -sFULL_ES3 -sINITIAL_HEAP=1024MB -sMAX_WEBGL_VERSION=2 --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/resources")
endif()

target_include_directories(viewgl
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/cxxopts/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../glad/include/glad)

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_libraries(viewgl
    PRIVATE
    ${LIBS}
    assimp
    view)
else()
    target_link_libraries(viewgl
    PRIVATE
    ${LIBS}
    glfw
    assimp
    view)
endif()
