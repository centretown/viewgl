#include "glad.h"
#include <GLFW/glfw3.h>

// #include <GLES2/gl2.h>
// #include <GLES3/gl3.h>

#include <cstring>
#include <glm/ext/matrix_transform.hpp>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include <stb_image.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "app.hpp"
#include "shader.hpp"
#include "win.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

float windowWidth = (float)SCREEN_WIDTH;
float windowHeight = (float)SCREEN_HEIGHT;

GLFWwindow *window;
App app;

void Loop();

int main(int argc, const char **argv) {

  app.Parse(argc, argv);
  window = app.InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
  if (window == NULL) {
    return 1;
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return 1;
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);

#ifdef WEBAPP
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif

  ImGui_ImplOpenGL3_Init(app.state.glslVersion);

  app.LoadFonts();
  app.InitSkyBox();
  app.LoadModels();

  viewgl::Shader &skyboxShader = app.SkyboxShader();
  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

#ifdef WEBAPP
  io.IniFilename = nullptr;
  printf("WEBAPP\n");
  emscripten_set_main_loop(Loop, 60, 1);
#else
  printf("NATIVE\n");
  while (!glfwWindowShouldClose(window)) {
    Loop();
  }
#endif

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void Loop() {

#ifndef WEBAPP
  if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
    ImGui_ImplGlfw_Sleep(10);
    return;
  }
#endif // WEBAPP

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (app.showModel) {
    app.DrawModel();
  }

  if (app.showSkybox) {
    glDepthFunc(GL_LEQUAL);
    app.DrawSkybox();
    glDepthFunc(GL_LESS);
  } // set depth function back to default

  app.CheckPanel();
#ifndef WEBAPP
  glfwSwapBuffers(window);
#endif // WEBAPP
  glfwPollEvents();
}
