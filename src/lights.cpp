#include "lights.hpp"

glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

void InitializeLights(Shader &targetShader, Camera &camera) {

  targetShader.use();
  targetShader.setVec3("viewPos", camera.Position);
  // targetShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
  targetShader.setFloat("material.shininess", 64.0f);

  // point light 1
  targetShader.setVec3("pointLights[0].position", pointLightPositions[0]);
  targetShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[0].constant", 1.0f);
  targetShader.setFloat("pointLights[0].linear", 0.09f);
  targetShader.setFloat("pointLights[0].quadratic", 0.032f);
  // point light 2
  targetShader.setVec3("pointLights[1].position", pointLightPositions[1]);
  targetShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[1].constant", 1.0f);
  targetShader.setFloat("pointLights[1].linear", 0.09f);
  targetShader.setFloat("pointLights[1].quadratic", 0.032f);
  // point light 3
  targetShader.setVec3("pointLights[2].position", pointLightPositions[2]);
  targetShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[2].constant", 1.0f);
  targetShader.setFloat("pointLights[2].linear", 0.09f);
  targetShader.setFloat("pointLights[2].quadratic", 0.032f);
  // point light 4
  targetShader.setVec3("pointLights[3].position", pointLightPositions[3]);
  targetShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
  targetShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
  targetShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("pointLights[3].constant", 1.0f);
  targetShader.setFloat("pointLights[3].linear", 0.09f);
  targetShader.setFloat("pointLights[3].quadratic", 0.032f);
  // spotLight
  targetShader.setVec3("spotLight.position", camera.Position);
  targetShader.setVec3("spotLight.direction", camera.Front);
  targetShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
  targetShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
  targetShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
  targetShader.setFloat("spotLight.constant", 1.0f);
  targetShader.setFloat("spotLight.linear", 0.09f);
  targetShader.setFloat("spotLight.quadratic", 0.032f);
  targetShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
  targetShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}

void UpdateLights(Shader &targetShader, Camera &camera) {
  targetShader.use();
  targetShader.setVec3("viewPos", camera.Position);
  targetShader.setVec3("spotLight.position", camera.Position);
  targetShader.setVec3("spotLight.direction", camera.Front);
}
