#pragma once
#include "camera.hpp"
#include "shader.hpp"

namespace viewgl {

void InitializeLights(Shader &targetShader, Camera &camera);
void UpdateLights(Shader &targetShader, Camera &camera);

} // namespace viewgl
