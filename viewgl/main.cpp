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
#include "input_options.hpp"
#include "model.hpp"
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

void DrawContainers(float scale, Shader &shader, unsigned int texture, int VAO);
void DrawGui(GLFWwindow *window);

// #define USE_OPEN_GLES
#if defined(USE_OPEN_GLES)
#define GLSL_VERSION 100
#elif defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif // USE_OPEN_GLES

#ifdef USE_OPEN_GLES
Shader curShader("../assets/shaders/gls100/depth.vert",
                 "../assets/shaders/gls100/depth.frag");
Shader skyboxShader("../assets/shaders/gls100/skybox.vert",
                    "../assets/shaders/gls100/skybox.frag");
#else
Shader curShader("../assets/shaders/gls330/depth.vert",
                 "../assets/shaders/gls330/depth.frag");
Shader skyboxShader("../assets/shaders/gls330/skybox.vert",
                    "../assets/shaders/gls330/skybox.frag");
#endif

// bool show_demo_window = true;
// bool show_another_window = false;
// ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// ImGuiIO *io = NULL;

void LoadFonts();

WinState state;
Camera camera;

int main(int argc, const char **argv) {

  InputOptions args;
  args.Parse("viewgl", argc, argv);
  printf("modelPath=\"%s\" skyboxPath=\"%s\"\n", args.modelPath.c_str(),
         args.skyboxPath.c_str());

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

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
  ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
  const char *glsl_version = "#version 330 core";
  ImGui_ImplOpenGL3_Init(glsl_version);

  curShader.Build();
  skyboxShader.Build();
  if (!curShader.IsValid() || !skyboxShader.IsValid()) {
    glfwTerminate();
    return -1;
  }

  LoadFonts();

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

  printf("modelPath=\"%s\" skyboxPath=\"%s\"\n", args.modelPath.c_str(),
         args.skyboxPath.c_str());

  unsigned int cubemapTexture = LoadCubemap(args.skyboxPath);

  Model curModel(args.modelPath);
  // calculate scale
  float scale = 1.0;
  float diffx = curModel.max.x - curModel.min.x;
  float diffy = curModel.max.y - curModel.min.y;
  float diff = fmax(diffx, diffy);
  if (diff != 0.0f)
    scale = 1.0 / diff;
  float scaleY = 1.0 / diffy;
  diffy = (curModel.min.y + diffy) * scaleY;
  diffx /= 2.0f;

  // shader configuration
  // --------------------
  curShader.use();
  curShader.setInt("texture1", 0);

  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

  while (!glfwWindowShouldClose(window)) {
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    curShader.use();
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    glm::mat4 view = camera.GetViewMatrix(axis);
    glm::mat4 projection =
        camera.GetProjectionMatrix(state.width, state.height);

    curShader.setMat4("projection", projection);
    curShader.setMat4("view", view);
    curShader.setVec3("cameraPos", camera.position);
    curShader.setInt("refraction", 1);
    curShader.setFloat("refractionIndex", state.refractionIndex);

    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    // translate it down so it's at the center of the scene
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    // it's a bit too big for our scene, so scale it down
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    // rotate about y-axis
    // model = glm::rotate(model, glm::radians(rotation_angle), axis);
    curShader.setMat4("model", model);

    curModel.Draw(curShader);

    // draw skybox as last
    glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when
                            // values are equal to depth buffer's content
    skyboxShader.use();
    view = glm::mat4(glm::mat3(
        camera.GetViewMatrix(axis))); // remove translation from the view matrix
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default

    if (state.CheckPanel()) {
      glViewport(state.panelWidth, 0, state.width, state.height);
      DrawGui(window);
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

void DrawGui(GLFWwindow *window) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  float fontSize = ImGui::GetFontSize();
  ImGui::SetNextWindowPos(ImVec2(0, 0));

  float panelWidth = fontSize * state.panelWidth + state.panelPadding;
  ImGui::SetNextWindowSize(
      ImVec2(panelWidth - state.panelPadding, state.height));

  bool bopen = false;
  if (ImGui::Begin("xxxPanelxxx", &bopen,
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_AlwaysAutoResize)) {
    if (ImGui::CollapsingHeader("Panel Settings")) {
      ImGui::SliderFloat("Panel Width", &state.panelWidth, PanelWidthMin,
                         PanelWidthMax, "%.0f");
      ImGui::SliderFloat("Refraction Index", &state.refractionIndex,
                         RefractionIndexMin, RefractionIndexMax, "%.1f");
    }
    if (ImGui::CollapsingHeader("Style Editor"))
      ImGui::ShowStyleEditor();
    ImGui::End();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void LoadFonts() {
  ImGuiIO &io = ImGui::GetIO();

  io.Fonts->AddFontDefault();

  io.Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16.0f);
  io.Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 18.0f);
  ImFont *font = io.Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20.0f);
  io.Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 22.0f);
  io.Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24.0f);
  io.Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 28.0f);
  io.Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 32.0f);
  if (font)
    io.FontDefault = font;
}
