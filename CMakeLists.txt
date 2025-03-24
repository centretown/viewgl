cmake_minimum_required(VERSION 3.0...3.28)
cmake_policy(VERSION 3.0...3.28)

project(viewgl)

# options
set(CMAKE_BUILD_TYPE Debug)

message(STATUS "Host system processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")
IF(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(CMAKE_CXX_FLAGS -DUSE_OPEN_GLES)
ENDIF()

set(CMAKE_CXX_STANDARD 17) # this does nothing for MSVC, use target_compile_options below
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
 "${CMAKE_SOURCE_DIR}/cmake/modules/")

# find the required packages
find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

# find_package(ASSIMP REQUIRED)
# message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")

if(WIN32)
    set(LIBS glfw3 opengl32 freetype irrKlang)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic")
    find_package(OpenGL REQUIRED)
    add_definitions(${OPENGL_DEFINITIONS})
    find_package(X11 REQUIRED)
    # note that the order is important for setting the libs
    # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
    set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype z)
    set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    FIND_LIBRARY(IOKit_LIBRARY IOKit)
    FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
    MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
    SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
    SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
    set(LIBS ${LIBS} ${APPLE_LIBS})
else()
    set(LIBS )
endif(WIN32)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(viewgl)
