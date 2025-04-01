#pragma once
#include "model.hpp"

#include <filesystem>

namespace viewgl {
struct Options {

  // input arguments
  std::string resourceBase = "/home/dave/src/viewgl/run/resources/";
  std::string modelName = "sphereofthedark";
  std::string skyboxName = "islands";
  std::string modelType = "stl";

  // source paths
  std::filesystem::path modelPath;
  std::filesystem::path skyboxPath;

  // selectable locations
  std::filesystem::path stlDirectory;
  std::filesystem::path objectDirectory;
  std::filesystem::path shaderDirectory;
  std::filesystem::path skyboxDirectory;

  float scale = 1.0f;

  Model model;
  unsigned int skyboxTexture;

  glm::vec3 Scale() { return glm::vec3(scale, scale, scale); }

  void DrawGui();
  int LoadModel();
  int LoadSkybox();
  int Parse(const char *title, int argc, const char **argv, bool log = true);

  static float skyboxVertices[];
  static size_t skyboxVerticesSize;
};

// void LoadList(std::filesystem::path &dir,
//               std::vector<std::filesystem::path> &list, bool log = true);

} // namespace viewgl
