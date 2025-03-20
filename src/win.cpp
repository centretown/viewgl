
#include "win.hpp"
#include "camera.hpp"
#include "glad.h"
#include <GLFW/glfw3.h>

static Camera *currentCamera = NULL;
static WinState *winState = NULL;
static float lastX = 0.0f;
static float lastY = 0.0f;
static bool firstMouse = true;
static bool leftButtonDown = false;
static bool rightButtonDown = false;
static bool panelToggle = false;
static bool startPressed = false;
static bool f2Pressed = false;

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  leftButtonDown = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
  panelToggle = (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE);
  // panelToggle = (!rightDown && rightButtonDown);
  // rightButtonDown = rightDown;
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  if (winState->showPanel)
    return;

  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;
  if (leftButtonDown)
    currentCamera->ProcessMovement(-xoffset, -yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (winState->showPanel)
    return;
  currentCamera->ProcessScroll(static_cast<float>(yoffset));
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  winState->Resize(width, height);
  glViewport(0, 0, width, height);
}

GLFWwindow *WinState::InitWindow(Camera *cam, int w, int h) {
  winState = this;
  currentCamera = cam;

  Resize(w, h);
  glfwInit();

#ifdef USE_OPEN_GLES
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
  window = glfwCreateWindow(width, height, glsl_version, NULL, NULL);
  if (!window) {
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    window = glfwCreateWindow(width, height, glsl_version, NULL, NULL);
    if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
  }
#else
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100 (WebGL 1.0)
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  const char *glsl_version = "#version 300 es";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#elif defined(IMGUI_IMPL_OPENGL_GL320)
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#else
  const char *glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // IMGUI_IMPL_OPENGL_ES2
  window = glfwCreateWindow(width, height, glsl_version, NULL, NULL);
  if (window == NULL) {
    return window;
  }
#endif // USE_OPEN_GLES

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  window = glfwCreateWindow(width, height, glsl_version, NULL, NULL);
  if (window == NULL) {
    return window;
  }
#endif

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  return window;
}

bool WinState::CheckPanel() {
  if (panelToggle) {
    showPanel = !showPanel;
    panelToggle = false;
  } else {
    if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
      int buttonCount;
      const unsigned char *buttons =
          glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

      if (startPressed && buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_RELEASE) {
        showPanel = !showPanel;
        startPressed = false;
      } else
        startPressed = (buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS);
    }
    if (f2Pressed && glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE) {
      showPanel = !showPanel;
      f2Pressed = false;
    } else
      f2Pressed = (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS);
  }
  return showPanel;
}

void WinState::ProcessInput(Camera &camera) {
  float currentFrame = glfwGetTime();
  float deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  CameraMovement direction = (CameraMovement)-1;
  float rotationAngleY = camera.RotationAngle();

  if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
    int axesCount;
    const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
    int buttonCount;
    const unsigned char *buttons =
        glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

    if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS)
      direction = CAMERA_FORWARD;
    if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS)
      direction = CAMERA_BACKWARD;
    if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS)
      rotationAngleY -= 1.0;
    if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS)
      rotationAngleY += 1.0;

    // GLFW_GAMEPAD_BUTTON_DPAD_RIGHT
    // GLFW_GAMEPAD_BUTTON_DPAD_DOWN
    // GLFW_GAMEPAD_BUTTON_DPAD_LEFT
    // int hatCount;
    // const unsigned char *hats = glfwGetJoystickHats(GLFW_JOYSTICK_1,
    // &hatCount);

    // printf("axes=%d buttons=%d hats=%d\n", axesCount, buttonCount, hatCount);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    direction = CAMERA_FORWARD;
  // ProcessDirection(CAMERA_FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    direction = CAMERA_BACKWARD;

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    rotationAngleY -= 1.0;

  if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
    rotationAngleY += 1.0;

  if (direction >= 0)
    camera.ProcessDirection(direction, deltaTime);
  camera.SetRotationAngle(rotationAngleY);
}
