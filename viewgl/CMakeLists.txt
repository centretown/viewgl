project(${CMAKE_PROJECT_NAME})

#set(CMAKE_CXX_FLAGS -DUSE_OPEN_GLES)

add_executable(viewgl
    main.cpp
    drawgui.cpp
    ../src/model.cpp
    ../src/mesh.cpp)

include_directories(
    .
    ../external/imgui
    ../external/imgui/backends
    ${ASSIMP_INCLUDE_DIR})

target_link_libraries(viewgl
    PRIVATE
    ${LIBS}
    assimp
    imguigl
    davegl)
