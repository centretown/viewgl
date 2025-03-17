#pragma once
#include "camera.hpp"
#include "shader.hpp"

void InitializeLights(Shader &targetShader, Camera &camera);
void UpdateLights(Shader &targetShader, Camera &camera);
