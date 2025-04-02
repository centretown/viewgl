
#include "win.hpp"
#include "camera.hpp"
#include "glad.h"
#include <GLFW/glfw3.h>

#include <cstdio>
#include <errno.h>

namespace viewgl {

static Camera *currentCamera = NULL;
static WinState *winState = NULL;

static float lastX = 0.0f;
static float lastY = 0.0f;
static bool firstMouse = true;
static bool leftMouseDown = false;
static bool mouseMenuClicked = false;
static int menuButton = GLFW_GAMEPAD_BUTTON_START;
static bool menuButtonPressed = false;
static bool menuKeyPressed = false;
static int menuKey = GLFW_KEY_F2;

// Set axis deadzones
const float leftDeadzoneX = 0.1f;
const float leftDeadzoneY = 0.1f;
const float rightDeadzoneX = 0.1f;
const float rightDeadzoneY = 0.1f;
const float leftTriggerDeadzone = -0.9f;
const float rightTriggerDeadzone = -0.9f;

int activeGamepPad = GLFW_JOYSTICK_1;

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  leftMouseDown = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
  mouseMenuClicked =
      (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE);
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
  if (leftMouseDown)
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

void error_callback(int errcode, const char *message) {
  printf("error code '%d' message: '%s'\n", errno, message);
}

void handleError() {
  int code = glfwGetError(NULL);
  switch (code) {
  case GLFW_NOT_INITIALIZED:
    printf("code = '%d': GLFW_NOT_INITIALIZED\n", code);
  case GLFW_INVALID_ENUM:
    printf("code = '%d': GLFW_INVALID_ENUM\n", code);
  case GLFW_INVALID_VALUE:
    printf("code = '%d': GLFW_INVALID_VALUE\n", code);
  case GLFW_API_UNAVAILABLE:
    printf("code = '%d': GLFW_API_UNAVAILABLE\n", code);
  case GLFW_VERSION_UNAVAILABLE:
    printf("code = '%d': GLFW_VERSION_UNAVAILABLE\n", code);
  case GLFW_FORMAT_UNAVAILABLE:
    printf("code = '%d': GLFW_FORMAT_UNAVAILABLE\n", code);
  case GLFW_NO_WINDOW_CONTEXT:
    printf("code = '%d': GLFW_NO_WINDOW_CONTEXT\n", code);
  case GLFW_PLATFORM_ERROR:
    printf("code = '%d': GLFW_PLATFORM_ERROR\n", code);
  }
}

#ifdef WEBAPP
#define USE_OPENGL_ES3
#endif //  WEBAPP

#if defined(USE_OPENGL_ES2)
#define IMGUI_IMPL_OPENGL_ES2
#elif defined(USE_OPENGL_ES3)
#define IMGUI_IMPL_OPENGL_ES3
#elif defined(USE_OPENGL_GL320)
#define IMGUI_IMPL_OPENGL_GL320
#else
#define IMGUI_IMPL_OPENGL_GL330
#endif //  USE_OPENGL_ES2

GLFWwindow *WinState::InitWindow(Camera *cam, int w, int h) {
  winState = this;
  currentCamera = cam;

  Resize(w, h);
  glfwInit();

#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100 (WebGL 1.0)
  glslVersion = "#version 100";
  glsDirectory = "gls100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
  // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
  glslVersion = "#version 300 es";
  glsDirectory = "gls100";
  // glsDirectory = "gls300";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  glslVersion = "#version 150";
  glsDirectory = "gls300";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#elif defined(IMGUI_IMPL_OPENGL_GL320)
  // GL 3.0 + GLSL 130
  glslVersion = "#version 130";
  glsDirectory = "gls300";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#else
  glslVersion = "#version 330";
  glsDirectory = "gls330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif // IMGUI_IMPL_OPENGL_ES2

  printf("glsl version: '%s' shader directory: '%s'\n", glslVersion,
         glsDirectory);

  window = glfwCreateWindow(width, height, glslVersion, NULL, NULL);
  if (window == NULL) {
    handleError();
    return window;
  }

  glfwSetErrorCallback(error_callback);
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  return window;
}

bool WinState::PanelActive() {
  if (mouseMenuClicked) {
    showPanel = !showPanel;
    mouseMenuClicked = false;
  } else {
    // #ifndef WEBAPP
    if (glfwJoystickIsGamepad(activeGamepPad)) {
      GLFWgamepadstate gamepadState = {0};
      glfwGetGamepadState(activeGamepPad, &gamepadState);
      if (menuButtonPressed &&
          gamepadState.buttons[menuButton] == GLFW_RELEASE) {
        showPanel = !showPanel;
        menuButtonPressed = false;
      } else
        menuButtonPressed = (gamepadState.buttons[menuButton] == GLFW_PRESS);
    }
    // #endif // WEBAPP
    if (menuKeyPressed && glfwGetKey(window, menuKey) == GLFW_RELEASE) {
      showPanel = !showPanel;
      menuKeyPressed = false;
    } else
      menuKeyPressed = (glfwGetKey(window, menuKey) == GLFW_PRESS);
  }
  return showPanel;
}

void WinState::ProcessInput(Camera &camera) {

  float currentFrame = glfwGetTime();
  float deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  CameraMovement direction = (CameraMovement)-1;
  float rotationAngleY = camera.RotationAngle();

  // #ifndef WEBAPP
  if (glfwJoystickIsGamepad(activeGamepPad)) {
    GLFWgamepadstate gamepadState = {0};
    glfwGetGamepadState(activeGamepPad, &gamepadState);
    if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS)
      direction = CAMERA_BACKWARD;
    if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS)
      direction = CAMERA_FORWARD;
    if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS)
      rotationAngleY -= 1.0;
    if (gamepadState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS)
      rotationAngleY += 1.0;
    //
    float value = gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    if (value < -leftDeadzoneX || value > leftDeadzoneX)
      rotationAngleY += value;
    value = gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    if (value < -leftDeadzoneX || value > leftDeadzoneX)
      direction = (value > 0) ? CAMERA_FORWARD : CAMERA_BACKWARD;
    //
    float moveX = 0.0f;
    float moveY = 0.0f;
    value = gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    if (value < -rightDeadzoneX || value > rightDeadzoneX)
      moveX = value;
    //
    value = gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    if (value < -rightDeadzoneY || value > rightDeadzoneY)
      moveY = value;
    //
    if (moveY != 0.0f || moveX != 0.0f) {
      // printf("%f,%f\n", moveX, moveY);
      camera.ProcessMovement(-moveX / camera.Sensitivity(),
                             moveY / camera.Sensitivity());
    }
  }
  // #endif // WEBAPP

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    direction = CAMERA_BACKWARD;

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    direction = CAMERA_FORWARD;

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    rotationAngleY -= 1.0;

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    rotationAngleY += 1.0;

  if (direction >= 0)
    camera.ProcessDirection(direction, deltaTime);
  camera.SetRotationAngle(rotationAngleY);
}
} // namespace viewgl
