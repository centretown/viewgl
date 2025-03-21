#pragma once
#include <cxxopts.hpp>

#include <filesystem>
#include <vector>

const std::string shaderDir = "shaders";
const std::string textureDir = "textures";
const std::string modelDir = "objects";
const std::string skyboxDir = "skybox";
const std::string stlDir = "stl";
const std::string subDir = "";

struct Options {
  std::string resourceDir = "../resources";
  std::string modelType = "stl";
  std::string modelName = "sphereofthedark";
  std::string skyboxName = "islands";

  std::filesystem::path modelPath;
  std::filesystem::path skyboxPath;

  std::filesystem::path stlPath;
  std::filesystem::path objectPath;
  std::filesystem::path shaderPath;
  std::filesystem::path skyboxPaths;

  std::vector<std::filesystem::path> objectList;
  std::vector<std::filesystem::path> stlList;
  std::vector<std::filesystem::path> skyboxList;
  std::vector<std::filesystem::path> shaderList;

  int Parse(const char *title, int argc, const char **argv, bool log = true);
};

void FillVector(std::filesystem::path &dir,
                std::vector<std::filesystem::path> &list, bool log = true);
