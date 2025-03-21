#include "camera.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>

namespace davegl {
void Camera::ProcessDirection(CameraMovement direction, float deltaTime) {
  float velocity = speed * deltaTime;
  if (direction == CAMERA_FORWARD)
    position += front * velocity;
  if (direction == CAMERA_BACKWARD)
    position -= front * velocity;
  if (direction == CAMERA_LEFT)
    position -= right * velocity;
  if (direction == CAMERA_RIGHT)
    position += right * velocity;
}

void Camera::ProcessMovement(float xoffset, float yoffset,
                             GLboolean constrainPitch) {
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (constrainPitch) {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateVectors();
}

void Camera::ProcessScroll(float yoffset) {
  zoom -= (float)yoffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 90.0f)
    zoom = 90.0f;
}

void Camera::updateVectors() {
  glm::vec3 f;
  f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  f.y = sin(glm::radians(pitch));
  f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(f);
  right = glm::normalize(glm::cross(front, worldUp));
  up = glm::normalize(glm::cross(right, front));
}

// void Camera::ProcessInput(GLFWwindow *window) {
//   float currentFrame = glfwGetTime();
//   float deltaTime = currentFrame - lastFrame;
//   lastFrame = currentFrame;

//   int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
//   if (present) {
//     int axesCount;
//     const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
//     int buttonCount;
//     const unsigned char *buttons =
//         glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
//     if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS) {
//       ProcessDirection(CAMERA_FORWARD, deltaTime);
//     }
//     if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS) {
//       ProcessDirection(CAMERA_BACKWARD, deltaTime);
//     }
//     if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS) {
//       rotationAngleY -= 1.0;
//     }
//     if (buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS) {
//       rotationAngleY += 1.0;
//     }
//     // GLFW_GAMEPAD_BUTTON_DPAD_RIGHT
//     // GLFW_GAMEPAD_BUTTON_DPAD_DOWN
//     // GLFW_GAMEPAD_BUTTON_DPAD_LEFT
//     // int hatCount;
//     // const unsigned char *hats = glfwGetJoystickHats(GLFW_JOYSTICK_1,
//     // &hatCount);

//     // printf("axes=%d buttons=%d hats=%d\n", axesCount, buttonCount,
//     hatCount);
//   }

//   if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
//     ProcessDirection(CAMERA_FORWARD, deltaTime * 5);
//   if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
//     ProcessDirection(CAMERA_BACKWARD, deltaTime * 5);
//   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
//       glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
//     ProcessDirection(CAMERA_FORWARD, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
//       glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
//     ProcessDirection(CAMERA_BACKWARD, deltaTime);

//   if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
//     rotationAngleY -= 1.0;
//   }
//   if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
//     rotationAngleY += 1.0;
//   }

//   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
//       glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
//     ProcessDirection(CAMERA_LEFT, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
//       glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
//     ProcessDirection(CAMERA_RIGHT, deltaTime);
// }
} // namespace davegl
