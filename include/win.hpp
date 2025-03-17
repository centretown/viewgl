#pragma once

#include "camera.hpp"
#include <GLFW/glfw3.h>


struct WinState
{
    Camera *camera;

    float lastX = DEFAULT_SCREEN_WIDTH / 2.0f;
    float lastY = DEFAULT_SCREEN_HEIGHT / 2.0f;
    bool firstMouse = true;
    bool leftButtonDown = false;

    WinState() {}

    WinState(Camera *camera, int width, int height): camera{camera}
    {
        lastX = width / 2.0f;
        lastY = height / 2.0f;
        camera->ResizeWindow(width,height);
    }
};

GLFWwindow *InitWindow(Camera *camera, int width, int height);
