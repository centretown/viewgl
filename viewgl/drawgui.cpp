#include "drawgui.hpp"

#include <filesystem>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void ShowFiles(std::vector<typename std::filesystem::path> list,
               std::string &selected_idx) {
  for (std::filesystem::path &p : list) {
    const bool is_selected = (selected_idx == p);
    if (ImGui::Selectable(p.c_str(), is_selected))
      selected_idx = p;
    if (is_selected)
      ImGui::SetItemDefaultFocus();
  }
}

void DrawGui(WinState &state, Options &options) {

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
    if (ImGui::CollapsingHeader("Model")) {

      ImGui::LabelText("Resource Directory", "%s", options.resourceDir.c_str());
      ImGui::LabelText("Type", "%s", options.modelType.c_str());
      ImGui::LabelText("Name", "%s", options.modelName.c_str());
      ImGui::LabelText("Skybox", "%s", options.skyboxName.c_str());

      if (ImGui::CollapsingHeader("STL Documents")) {
        static std::string selected_stl;
        ShowFiles(options.stlList, selected_stl);
      }
      if (ImGui::CollapsingHeader("Objects")) {
        static std::string selected_obj;
        ShowFiles(options.objectList, selected_obj);
      }
      if (ImGui::CollapsingHeader("Skyboxes")) {
        static std::string selected_skybox;
        ShowFiles(options.skyboxList, selected_skybox);
      }
      if (ImGui::CollapsingHeader("Shaders")) {
        static std::string selected_shader;
        ShowFiles(options.shaderList, selected_shader);
      }
    }

    if (ImGui::CollapsingHeader("Panel Settings")) {
      ImGui::SliderFloat("Panel Width", &state.panelWidth, PanelWidthMin,
                         PanelWidthMax, "%.0f");
      ImGui::SliderFloat("Refraction Index", &state.refractionIndex,
                         RefractionIndexMin, RefractionIndexMax, "%.1f");
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
