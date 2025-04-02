#include "app.hpp"
#include "texture.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void App::Parse(int argc, const char **argv) {
  options.Parse("viewgl", argc, argv);
  printf("resourcePath=\"%s\"\n modelPath=\"%s\"\n skyboxPath=\"%s\"\n",
         options.resourceBase.c_str(), options.modelPath.c_str(),
         options.skyboxPath.c_str());
}

GLFWwindow *App::InitWindow(int width, int height) {
  window = state.InitWindow(&camera, width, height);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
  }
  return window;
}

void App::InitSkyBox() {
  // skyboxVAO VAO
  printf("glGenVertexArrays\n");
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, options.skyboxVerticesSize,
               &options.skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  printf("modelPath=\"%s\" skyboxPath=\"%s\"\n", options.modelPath.c_str(),
         options.skyboxPath.c_str());

  options.skyboxTexture = viewgl::LoadCubemap(options.skyboxPath);
}

void App::LoadModels() { options.LoadModel(); }

void App::LoadFonts() {
  auto base = options.resourceBase;
  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontDefault();
  std::string fontLocation = base + "/fonts/DejaVuSans.ttf";
  printf("fontLocation %s\n", fontLocation.c_str());
  float sizes[] = {
      16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 28.0f, 32.0f,
  };
  ImFont *font;
  for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
    font = io.Fonts->AddFontFromFileTTF(fontLocation.c_str(), sizes[i]);
    if (font && sizes[i] == 20.0f)
      io.FontDefault = font;
  }
}

bool App::LoadShaders() {
  std::filesystem::path shaderPath = options.shaderDirectory;
  shaderPath.append(state.glsDirectory);
  char filename[32] = {0};

  for (int i = 0; i < APP_SHADER_COUNT; i++) {
    AppShader &ash = shaders[i];
    viewgl::Shader &shader = ash.shader;
    std::filesystem::path vertPath = shaderPath;
    snprintf(filename, sizeof(filename), "%s.vert", ash.name.c_str());
    vertPath.append(filename);

    std::filesystem::path fragPath = shaderPath;
    snprintf(filename, sizeof(filename), "%s.frag", ash.name.c_str());
    fragPath.append(filename);

    shader.SetPaths(vertPath.c_str(), fragPath.c_str());
    shader.Build();
    if (!shader.IsValid()) {
      return false;
    }
  }
  return true;
}

void App::DrawModel() {
  viewgl::Shader &depthShader = UseDepthShader();
  depthShader.use();
  glm::vec3 axis(0.0f, 1.0f, 0.0f);
  glm::mat4 view = camera.GetViewMatrix(axis);
  glm::mat4 projection = camera.GetProjectionMatrix(state.width, state.height);
  depthShader.setMat4("projection", projection);
  depthShader.setMat4("view", view);
  depthShader.setVec3("cameraPos", camera.position);
  depthShader.setInt("op", 0);
  depthShader.setFloat("refractionIndex", state.refractionIndex);

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
  model = glm::scale(model, options.Scale());
  depthShader.setMat4("model", model);
  options.model.Draw(depthShader);
}
void App::DrawSkybox() {
  viewgl::Shader &skyboxShader = UseShader(APP_SHADER_SKYBOX);
  skyboxShader.use();
  glm::vec3 axis(0.0f, 1.0f, 0.0f);
  // glm::mat4 view = camera.GetViewMatrix(axis);
  glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix(axis)));
  skyboxShader.setMat4("view", view);
  glm::mat4 projection = camera.GetProjectionMatrix(state.width, state.height);
  skyboxShader.setMat4("projection", projection);

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, options.skyboxTexture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

void App::CheckPanel() {
  if (state.PanelActive()) {
    float fontSize = ImGui::GetFontSize();
    glViewport(state.panelWidth * fontSize, 0, state.width, state.height);
    DrawPanel();
  } else {
    glViewport(0, 0, state.width, state.height);
    state.ProcessInput(camera);
  }
}

void App::DrawPanel() {

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

    if (ImGui::CollapsingHeader("Settings")) {
      ImGui::SliderFloat("Panel Width", &state.panelWidth,
                         viewgl::PanelWidthMin, viewgl::PanelWidthMax, "%.0f");

      if (ImGui::CollapsingHeader("Depth Shaders")) {
        for (int i = 0; i < APP_DEPTH_SHADER_COUNT; i++) {
          const bool is_selected = (depthShader == i);
          AppShader &ash = shaders[i];
          if (ImGui::Selectable(ash.name.c_str(), is_selected))
            depthShader = i;
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
      }

      ImGui::SliderFloat("Refraction Index", &state.refractionIndex,
                         viewgl::RefractionIndexMin, viewgl::RefractionIndexMax,
                         "%.1f");
    }

    options.DrawGui();

    // always end here.
    if (ImGui::CollapsingHeader("Style Editor"))
      ImGui::ShowStyleEditor();
    ImGui::End();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
