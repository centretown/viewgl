#include "drawgui.hpp"

#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void selectFromList(std::vector<std::filesystem::path> list,
                    std::string &selected) {

  for (std::filesystem::path &current : list) {
    const bool is_selected = (selected == current);
    if (ImGui::Selectable(current.stem().c_str(), is_selected))
      selected = current;
    if (is_selected)
      ImGui::SetItemDefaultFocus();
  }
}

void DrawGui(viewgl::WinState &state, viewgl::Options &options) {
  static std::string selected_stl;
  static std::string selected_obj;
  static std::string selected_type;
  static std::string selected_skybox;
  static std::string selected_shader;

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

    if (ImGui::CollapsingHeader("Options")) {

      ImGui::LabelText("Model", "%s", options.modelName.c_str());
      ImGui::LabelText("Type", "%s", options.modelType.c_str());
      ImGui::LabelText("Skybox", "%s", options.skyboxName.c_str());

      if (ImGui::CollapsingHeader("STL Documents", 0))
        selectFromList(options.stlList, selected_stl);
      if (selected_stl != "" && selected_stl != options.objectPath) {
        options.objectPath = selected_stl;
        options.modelType = options.objectPath.extension();
        options.modelName = options.objectPath.stem();
      }

      if (ImGui::CollapsingHeader("Objects"))
        selectFromList(options.objectList, selected_obj);
      if (selected_obj != "" && selected_obj != options.objectPath) {
        options.objectPath = selected_obj;
        options.modelType = "obj";
        options.modelName = options.objectPath.stem();
      }

      if (ImGui::CollapsingHeader("Skyboxes"))
        selectFromList(options.skyboxList, selected_skybox);
      if (ImGui::CollapsingHeader("Shaders"))
        selectFromList(options.shaderList, selected_shader);
    }

    if (ImGui::CollapsingHeader("Panel Settings")) {
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

void LoadFonts() {
  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontDefault();
  const char *fontLocation = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
  io.Fonts->AddFontFromFileTTF(fontLocation, 16.0f);
  io.Fonts->AddFontFromFileTTF(fontLocation, 18.0f);
  ImFont *font = io.Fonts->AddFontFromFileTTF(fontLocation, 20.0f);
  io.Fonts->AddFontFromFileTTF(fontLocation, 22.0f);
  io.Fonts->AddFontFromFileTTF(fontLocation, 24.0f);
  io.Fonts->AddFontFromFileTTF(fontLocation, 28.0f);
  io.Fonts->AddFontFromFileTTF(fontLocation, 32.0f);
  if (font)
    io.FontDefault = font;
}
