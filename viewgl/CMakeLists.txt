project(${CMAKE_PROJECT_NAME})

#set(CMAKE_CXX_FLAGS -DUSE_OPEN_GLES)

add_executable(viewgl
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/drawgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mesh.cpp)

target_include_directories(viewgl
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
