#include "app.hpp"
#include "draw.hpp"
#include "texture.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>

static std::vector<std::string> stlFilter = {".stl"};
static std::vector<std::string> objFilter = {".obj", ".dae", ".gltf"};
static std::vector<std::string> shaderFilter = {".frag"};

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
  options.shaderDirectory /= state.glsDirectory;
  printf("shaderDirectory=\"%s\"\n", options.resourceBase.c_str());
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
  skyboxTexture = viewgl::LoadCubemap(options.skyboxPath);
}

void App::LoadModels() { LoadModel(); }

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

bool fileUpdated(std::filesystem::path &p,
                 std::filesystem::file_time_type &current) {
  std::filesystem::file_time_type lastWritten =
      std::filesystem::last_write_time(p);
  if (current < lastWritten) {
    current = lastWritten;
    return true;
  }
  return false;
}

viewgl::Shader &App::GetShader(std::string &name) {
  std::filesystem::path vertPath = VertPath(name);
  std::filesystem::path fragPath = FragPath(name);

  ShaderItem &item = shaders[name];
  viewgl::Shader &shader = item.shader;
  if (fileUpdated(vertPath, item.writeTime) ||
      fileUpdated(fragPath, item.writeTime)) {
    shader.SetPaths(vertPath.c_str(), fragPath.c_str());
    shader.Build();
  }
  return shader;
}

void App::DrawModel() {
  viewgl::Shader &depthShader = DepthShader();
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
  model = glm::scale(model, Scale());
  depthShader.setMat4("model", model);
  currentModel.Draw(depthShader);
}

void App::DrawSkybox() {
  viewgl::Shader &skyboxShader = SkyboxShader();
  skyboxShader.use();
  glm::vec3 axis(0.0f, 1.0f, 0.0f);
  // glm::mat4 view = camera.GetViewMatrix(axis);
  glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix(axis)));
  skyboxShader.setMat4("view", view);
  glm::mat4 projection = camera.GetProjectionMatrix(state.width, state.height);
  skyboxShader.setMat4("projection", projection);

  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
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
  ImGui::SetNextWindowSize(ImVec2(panelWidth, state.height));

  bool bopen = false;
  if (ImGui::Begin("##Panel", &bopen,
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoCollapse |
                       ImGuiWindowFlags_AlwaysAutoResize)) {

    std::string modelLabel = "Model: ";
    modelLabel += options.modelPath.stem();
    ImGui::Checkbox(modelLabel.c_str(), &showModel);

    ImGui::Indent();
    std::filesystem::path modelPath = options.modelPath;
    if (ImGui::CollapsingHeader("Select STL Drawing", 0)) {
      SelectFile(options.stlDirectory, options.modelPath, stlFilter);
    }

    if (ImGui::CollapsingHeader("Select Object Model")) {
      SelectFile(options.objectDirectory, options.modelPath, objFilter);
    }

    if (modelPath != options.modelPath) {
      currentModel.Reload(options.modelPath);
    }

    std::filesystem::path shaderPath = FragPath(depthShaderName);
    if (ImGui::CollapsingHeader("Select Shader File")) {
      SelectFile(options.shaderDirectory, shaderPath, shaderFilter);
    }

    if (shaderPath != FragPath(depthShaderName)) {
      depthShaderName = shaderPath.filename().stem();
      printf("shaderPath '%s' new depthShaderName '%s'\n", shaderPath.c_str(),
             depthShaderName.c_str());
    }
    ImGui::Unindent();

    std::string skyboxLabel = "Skybox: ";
    skyboxLabel += options.skyboxPath.stem();
    ImGui::Checkbox(skyboxLabel.c_str(), &showSkybox);
    ImGui::Indent();
    if (ImGui::CollapsingHeader("Select Skybox##skybox")) {
      ImGui::Indent();
      auto skyboxPath = options.skyboxPath;
      SelectFolder(options.skyboxDirectory, options.skyboxPath);
      if (skyboxPath != options.skyboxPath)
        skyboxTexture = viewgl::LoadCubemap(options.skyboxPath);
      ImGui::Unindent();
    }
    ImGui::Unindent();

    if (ImGui::CollapsingHeader("Settings")) {
      ImGui::SliderFloat("Panel Width", &state.panelWidth,
                         viewgl::PanelWidthMin, viewgl::PanelWidthMax, "%.0f");

      ImGui::SliderFloat("Refraction Index", &state.refractionIndex,
                         viewgl::RefractionIndexMin, viewgl::RefractionIndexMax,
                         "%.1f");
    }

    // always end here.
    if (ImGui::CollapsingHeader("Style Editor"))
      ImGui::ShowStyleEditor();
    ImGui::End();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int App::LoadModel() {
  currentModel.SetPath(options.modelPath, options.resourceBase);
  currentModel.Load();
  scale = currentModel.Scale();
  return 0;
}
