#include "glad.h"
#include <GLFW/glfw3.h>

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

#include "camera.hpp"
#include "drawgui.hpp"
#include "model.hpp"
#include "options.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "win.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
float windowWidth = (float)SCREEN_WIDTH;
float windowHeight = (float)SCREEN_HEIGHT;

void DrawContainers(float scale, viewgl::Shader &shader, unsigned int texture,
                    int VAO);

// #define USE_OPEN_GLES
#if defined(USE_OPEN_GLES)
#define GLSL_VERSION 100
#elif defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif // USE_OPEN_GLES

viewgl::Shader shader;
viewgl::Shader skyboxShader;

const char *curVert = "depth.vert";
const char *curFrag = "depth.frag";

const char *skyVert = "skybox.vert";
const char *skyFrag = "skybox.frag";

#ifdef USE_OPEN_GLES
const char *glsDir = "gls100";
#else
const char *glsDir = "gls330";
#endif

float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

viewgl::WinState state;
viewgl::Camera camera;

int main(int argc, const char **argv) {

  viewgl::Options options;
  options.Parse("viewgl", argc, argv);
  printf("resourcePath=\"%s\"\n modelPath=\"%s\"\n skyboxPath=\"%s\"\n",
         options.resourceDir.c_str(), options.modelPath.c_str(),
         options.skyboxPath.c_str());

  GLFWwindow *window = state.InitWindow(&camera, SCREEN_WIDTH, SCREEN_HEIGHT);
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

#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif

  const char *glsl_version = "#version 330 core";
  ImGui_ImplOpenGL3_Init(glsl_version);

  std::filesystem::path shaderPath = options.shaderPath;
  shaderPath.append(glsDir);

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

  LoadFonts();

  // skyboxVAO VAO
  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  printf("modelPath=\"%s\" skyboxPath=\"%s\"\n", options.modelPath.c_str(),
         options.skyboxPath.c_str());

  options.skyboxTexture = viewgl::LoadCubemap(options.skyboxPath);
  options.LoadModel();

  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    glm::mat4 view = camera.GetViewMatrix(axis);
    glm::mat4 projection =
        camera.GetProjectionMatrix(state.width, state.height);

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setVec3("cameraPos", camera.position);
    shader.setInt("refraction", 1);
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

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
