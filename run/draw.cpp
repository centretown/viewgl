#include "draw.hpp"
#include "imgui.h"

bool Filter(const std::filesystem::path &entry,
            const std::vector<std::string> &filter) {
  for (auto match : filter)
    if (match == entry)
      return true;
  return false;
}

void SelectFolder(const std::filesystem::path &directory,
                  std::filesystem::path &current) {
  std::string suffix = "##";
  suffix += directory;

  ImGui::Indent();
  for (auto const &entry : std::filesystem::directory_iterator{directory}) {
    auto path = entry.path();
    auto key = path.filename();
    key += suffix;
    if (entry.is_directory()) {
      const bool is_selected = (path == current);
      if (ImGui::Selectable(key.c_str(), is_selected))
        current = path;
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
  }
  ImGui::Unindent();
}

void SelectFile(const std::filesystem::path &directory,
                std::filesystem::path &current,
                const std::vector<std::string> &filter) {
  std::string suffix = "##";
  suffix += directory;

  ImGui::Indent();
  for (auto const &entry : std::filesystem::directory_iterator{directory}) {
    auto path = entry.path();
    auto key = path.filename();
    key += suffix;

    if (entry.is_regular_file() && Filter(path.extension(), filter)) {
      const bool is_selected = (path == current);
      if (ImGui::Selectable(key.c_str(), is_selected))
        current = path;
      if (is_selected)
        ImGui::SetItemDefaultFocus();

    } else if (entry.is_directory()) {
      if (ImGui::CollapsingHeader(key.c_str(), 0)) {
        SelectFile(entry.path(), current, filter);
      }
    }
  }
  ImGui::Unindent();
}
