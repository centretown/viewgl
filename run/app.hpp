#pragma once
#include <map>

#include "model.hpp"
#include "options.hpp"
#include "shader.hpp"
#include "win.hpp"

struct ShaderItem {
  viewgl::Shader shader;
  std::filesystem::file_time_type writeTime =
      std::filesystem::file_time_type::min();
};

struct App {
  viewgl::Options options;
  viewgl::Camera camera;
  viewgl::WinState state;
  GLFWwindow *window = NULL;

  unsigned int skyboxVAO;
  unsigned int skyboxVBO;
  std::map<std::string, ShaderItem> shaders;

  unsigned int skyboxTexture;
  viewgl::Model currentModel;
  float scale = 1.0f;
  glm::vec3 Scale() { return glm::vec3(scale, scale, scale); }

  std::string depthShaderName = "base";
  std::string skyboxShaderName = "skybox";
  bool showModel = true;
  bool showSkybox = true;

  void Parse(int argc, const char **argv);
  GLFWwindow *InitWindow(int width, int height);
  void InitSkyBox();

  void DrawModel();
  void DrawSkybox();
  void CheckPanel();
  void DrawPanel();

  void LoadFonts();
  void LoadModels();
  int LoadModel();
  int LoadSkybox();
  viewgl::Shader &GetShader(std::string &name);

  viewgl::Shader &DepthShader() { return GetShader(depthShaderName); }
  viewgl::Shader &SkyboxShader() { return GetShader(skyboxShaderName); }
  std::filesystem::path VertPath(std::string &name) {
    return options.shaderDirectory / (name + ".vert");
  }
  std::filesystem::path FragPath(std::string &name) {
    return options.shaderDirectory / (name + ".frag");
  }
};
