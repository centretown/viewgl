#pragma once

#include "camera.hpp"
#include <GLFW/glfw3.h>

namespace davegl {

const float PanelWidthMin = 25.0;
const float PanelWidthMax = 50.0;

const float RefractionIndexMin = 1.0;
const float RefractionIndexMax = 10.0;

struct WinState {
  float width = 1.0f;  // pixel units
  float height = 1.0f; // pixel units
  GLFWwindow *window;

  // Camera *camera;
  bool showPanel = false;
  float panelWidth = 26.0f;   // font units
  float panelPadding = 10.0f; // pixel units
  int refraction = 0;
  float refractionIndex = 4.0f;

  float lastFrame = 0.0f; // Time of last frame

  GLFWwindow *InitWindow(Camera *camera, int width, int height);

  void Resize(int w, int h) {
    width = static_cast<float>(w);
    height = static_cast<float>(h);
  }
  bool PanelActive();
  void ProcessInput(Camera &camera);

private:
};
} // namespace davegl
