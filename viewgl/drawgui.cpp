#include "drawgui.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void DrawGui(viewgl::WinState &state, viewgl::Options &options) {

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

    options.DrawGui();

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
  float sizes[] = {
      16.0f, 18.0f, 20.0f, 22.0f, 24.0f, 28.0f, 32.0f,
  };
  ImFont *font;
  for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
    font = io.Fonts->AddFontFromFileTTF(fontLocation, sizes[i]);
    if (font && sizes[i] == 20.0f)
      io.FontDefault = font;
  }
}
