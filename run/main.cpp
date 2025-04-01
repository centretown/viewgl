#include "glad.h"
#include <GLFW/glfw3.h>

#include <cstring>
#include <filesystem>
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

#include "camera.hpp"
#include "drawgui.hpp"
#include "model.hpp"
#include "options.hpp"
#include "shader.hpp"
#include "texture.hpp"
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

void DrawContainers(float scale, viewgl::Shader &shader, unsigned int texture,
                    int VAO);

viewgl::Shader shader;
viewgl::Shader skyboxShader;

const char *curVert = "depth.vert";
const char *curFrag = "depth.frag";

const char *skyVert = "skybox.vert";
const char *skyFrag = "skybox.frag";

// #include <GLES2/gl2.h>
// #include <GLES3/gl3.h>

viewgl::WinState state;
viewgl::Camera camera;
GLFWwindow *window;
viewgl::Options options;
unsigned int skyboxVAO, skyboxVBO;

// #ifdef WEBAPP
// #include <emscripten/emscripten.h>
// void showfiles(std::filesystem::path p) {
// for (auto const &dir_entry : std::filesystem::directory_iterator{p}) {
// printf("%s\n", dir_entry.path().c_str());
// if (dir_entry.is_directory()) {
// showfiles(dir_entry);
// }
// }
// }
// display the file system
// #ifdef WEBAPP
// std::filesystem::path p("/");
// printf("files...\n");
// showfiles(p);
// printf("files end\n\n");
// #endif
// #endif

void Loop();

int main(int argc, const char **argv) {

  options.Parse("viewgl", argc, argv);
  printf("resourcePath=\"%s\"\n modelPath=\"%s\"\n skyboxPath=\"%s\"\n",
         options.resourceBase.c_str(), options.modelPath.c_str(),
         options.skyboxPath.c_str());

  window = state.InitWindow(&camera, SCREEN_WIDTH, SCREEN_HEIGHT);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    return 1;
  }

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return -1;
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

  ImGui_ImplOpenGL3_Init(state.glslVersion);

  std::filesystem::path shaderPath = options.shaderDirectory;
  shaderPath.append(state.glsDirectory);

  std::filesystem::path vertPath = shaderPath;
  vertPath.append("depth.vert");
  std::filesystem::path fragPath = shaderPath;
  fragPath.append("depth.frag");
  shader.SetPaths(vertPath.c_str(), fragPath.c_str());
  shader.Build();

  vertPath = shaderPath;
  vertPath.append("skybox.vert");
  fragPath = shaderPath;
  fragPath.append("skybox.frag");
  skyboxShader.SetPaths(vertPath.c_str(), fragPath.c_str());
  skyboxShader.Build();

  if (!shader.IsValid() || !skyboxShader.IsValid()) {
    glfwTerminate();
    return -1;
  }

  LoadFonts(options.resourceBase);
  printf("LoadFonts done\n");

  // skyboxVAO VAO
  printf("glGenVertexArrays\n");
  glGenVertexArrays(1, &skyboxVAO);
  printf("glGenBuffers\n");
  glGenBuffers(1, &skyboxVBO);
  printf("glBindVertexArray\n");
  glBindVertexArray(skyboxVAO);
  printf("glBindBuffer\n");
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  printf("glBufferData\n");
  glBufferData(GL_ARRAY_BUFFER, options.skyboxVerticesSize,
               &options.skyboxVertices, GL_STATIC_DRAW);
  printf("glEnableVertexAttribArray\n");
  glEnableVertexAttribArray(0);
  printf("glVertexAttribPointer\n");
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  printf("modelPath=\"%s\" skyboxPath=\"%s\"\n", options.modelPath.c_str(),
         options.skyboxPath.c_str());

  options.skyboxTexture = viewgl::LoadCubemap(options.skyboxPath);
  printf("LoadModel\n");
  options.LoadModel();
  printf("LoadModel done\n");

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

  shader.use();
  glm::vec3 axis(0.0f, 1.0f, 0.0f);
  glm::mat4 view = camera.GetViewMatrix(axis);
  glm::mat4 projection = camera.GetProjectionMatrix(state.width, state.height);

  shader.setMat4("projection", projection);
  shader.setMat4("view", view);
  shader.setVec3("cameraPos", camera.position);
  shader.setInt("op", 0);
  shader.setFloat("refractionIndex", state.refractionIndex);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
  model = glm::scale(model, options.Scale());
  shader.setMat4("model", model);
  options.model.Draw(shader);

  glDepthFunc(GL_LEQUAL);
  skyboxShader.use();
  view = glm::mat4(glm::mat3(camera.GetViewMatrix(axis)));
  skyboxShader.setMat4("view", view);
  skyboxShader.setMat4("projection", projection);

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, options.skyboxTexture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default

  if (state.PanelActive()) {
    glViewport(state.panelWidth, 0, state.width, state.height);
    DrawGui(state, options);
  } else {
    glViewport(0, 0, state.width, state.height);
    state.ProcessInput(camera);
  }
#ifndef WEBAPP
  glfwSwapBuffers(window);
#endif // WEBAPP
  glfwPollEvents();
}
