#ifndef CAMERA_H
#define CAMERA_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace viewgl {

enum CameraMovement {
  CAMERA_FORWARD,
  CAMERA_BACKWARD,
  CAMERA_LEFT,
  CAMERA_RIGHT
};

const float CAMERA_YAW = -90.0f;
const float CAMERA_PITCH = 0.0f;
const float CAMERA_SPEED = 2.5f;
const float CAMERA_SENSITIVITY = 0.1f;
const float CAMERA_ZOOM = 45.0f;

class Camera {
  // euler Angles
  float yaw = CAMERA_YAW;
  float pitch = CAMERA_PITCH;
  // camera options
  float speed = CAMERA_SPEED;
  float sensitivity = CAMERA_SENSITIVITY;
  float zoom = CAMERA_ZOOM;
  float lastFrame = 0.0f; // Time of last frame

  float rotationAngleY = 0.0f;
  float rotationAngleX = 0.0f;
  float rotationAngleZ = 0.0f;

public:
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 right = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

  Camera() { updateVectors(); }

  glm::mat4 GetViewMatrix(glm::vec3 axis) {
    return glm::rotate(glm::lookAt(position, position + front, up),
                       glm::radians(rotationAngleY), axis);
  }

  glm::mat4 RotateViewMatrix(glm::mat4 &view, glm::vec3 axis) {
    view = glm::lookAt(position, position + front, up);
    view = glm::rotate(view, glm::radians(rotationAngleY), axis);
    return view;
  }

  glm::mat4 GetProjectionMatrix(float width, float height) {
    return glm::perspective(glm::radians(zoom), width / height, 0.1f, 100.0f);
  }

  void ProcessDirection(CameraMovement direction, float deltaTime);
  void ProcessMovement(float xoffset, float yoffset,
                       GLboolean constrainPitch = true);
  void ProcessScroll(float yoffset);
  // void ProcessInput(GLFWwindow *window);

  float RotationAngle() { return rotationAngleY; }
  float Sensitivity() { return sensitivity; }
  void SetRotationAngle(float angleY) { rotationAngleY = angleY; }
  glm::vec3 &Position() { return position; };
  glm::vec3 &Front() { return front; };

private:
  void updateVectors();
};

} // namespace viewgl

#endif
