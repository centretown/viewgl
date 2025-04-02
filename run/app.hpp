#pragma once
#include <vector>

#include "options.hpp"
#include "shader.hpp"
#include "win.hpp"

enum APP_SHADER : int {
  APP_SHADER_BASE,
  APP_SHADER_REFLECT,
  APP_SHADER_REFRACT,
  APP_SHADER_SKYBOX,
  APP_SHADER_COUNT,
  APP_DEPTH_SHADER_COUNT = APP_SHADER_COUNT - 1,
};

struct AppShader {
  std::string name;
  viewgl::Shader shader;
  AppShader(std::string name) : name{name} {}
};

struct App {
  viewgl::WinState state;
  viewgl::Options options;
  viewgl::Camera camera;
  GLFWwindow *window = NULL;
  unsigned int skyboxVAO;
  unsigned int skyboxVBO;
  std::vector<AppShader> shaders;

  // viewgl::Shader shaders[APP_SHADER_COUNT];
  int depthShader = 0;

  App() {
    // std::vector<AppShader> shaders = {
    //     {.name = "base"},
    //     {.name = "refract"},
    //     {.name = "reflect"},
    //     {.name = "skybox"},
    // };
    shaders.push_back(AppShader("base"));
    shaders.push_back(AppShader("refract"));
    shaders.push_back(AppShader("reflect"));
    shaders.push_back(AppShader("skybox"));
  }

  void Parse(int argc, const char **argv);
  GLFWwindow *InitWindow(int width, int height);
  void InitSkyBox();

  void DrawModel();
  void DrawSkybox();
  void CheckPanel();
  void DrawPanel();

  bool LoadShaders();
  void LoadFonts();
  void LoadModels();

  viewgl::Shader &UseShader(int ShaderID) {
    assert(ShaderID < APP_SHADER_COUNT);
    return shaders[ShaderID].shader;
  }
  viewgl::Shader &UseDepthShader() { return shaders[depthShader].shader; }
};
