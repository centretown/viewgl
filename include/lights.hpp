#pragma once
#include "camera.hpp"
#include "shader.hpp"

namespace davegl {

void InitializeLights(Shader &targetShader, Camera &camera);
void UpdateLights(Shader &targetShader, Camera &camera);

} // namespace davegl
