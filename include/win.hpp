#pragma once

#include "camera.hpp"
#include <GLFW/glfw3.h>

const float PanelWidthMin = 25.0;
const float PanelWidthMax = 50.0;

const float RefractionIndexMin = 1.0;
const float RefractionIndexMax = 10.0;

struct WinState {
  GLFWwindow *window;
  Camera *camera;

  // float windowWidth = 0.0f;
  // float windowHeight = 0.0f;
  bool showPanel = false;
  float panelWidth = 36;   // font units
  float panelPadding = 10; // pixel units
  int refraction = 0;
  float refractionIndex = 4.0f;

  GLFWwindow *InitWindow(Camera *camera, int width, int height);
  float WindowWidth() { return camera->windowWidth; }
  float WindowHeight() { return camera->windowHeight; }

private:
};
