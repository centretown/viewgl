#include "options.hpp"
#include <cxxopts.hpp>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <string>

#include "imgui.h"
#include "mesh.hpp"
#include "texture.hpp"
namespace viewgl {

int Options::Parse(const char *title, int argc, const char **argv, bool log) {

#ifndef WEBAPP
  std::unique_ptr<cxxopts::Options> allocated(
      new cxxopts::Options(argv[0], title));

  cxxopts::Options &args = *allocated;
  args.set_width(150)
      .set_tab_expansion()
      .allow_unrecognised_options()
      .add_options() //
      ("r,resources", "Resource Directory",
       cxxopts::value<std::string>(), //
       resourceBase)                  //

      ("t,type", "Model Type eg. 'stl' 'model'", //
       cxxopts::value<std::string>(),
       modelType) //

      ("o,object", "Model File Name eg. 'enao.stl'",
       cxxopts::value<std::string>(),
       modelName) //

      ("s,skybox", "Skybox Directory eg. 'islands'",
       cxxopts::value<std::string>(),
       skyboxName) //

      ("h,help", "Print help"); //

  cxxopts::ParseResult result = args.parse(argc, argv);

  if (result.count("r")) {
    resourceBase = result["r"].as<std::string>().c_str();
  }

  if (result.count("o")) {
    modelName = result["o"].as<std::string>();
  }

  if (result.count("s")) {
    skyboxName = result["s"].as<std::string>();
  }

  if (result.count("t")) {
    modelType = result["t"].as<std::string>();
  }
#endif // #ifndef WEBAPP

  std::string dir;
  if (modelType == "stl")
    dir = stlDir;
  else
    dir = modelDir + "/" + modelName;

  modelPath = resourceBase + "/" + dir + "/" + modelName + "." + modelType;

  skyboxPath = resourceBase;
  skyboxPath /= skyboxDir;
  skyboxPath /= skyboxName;

  shaderDirectory = resourceBase;
  shaderDirectory /= shaderDir;

  objectDirectory = resourceBase + "/" + modelDir;
  stlDirectory = resourceBase + "/" + stlDir;
  skyboxDirectory = resourceBase + "/" + skyboxDir;

  printf("resourcePath='%s'\n"    //
         "modelPath='%s'\n"       //
         "skyboxPath='%s'\n"      //
         "shaderPath='%s'\n"      //
         "objectPath='%s'\n"      //
         "stlPath='%s'\n",        //
         resourceBase.c_str(),    //
         modelPath.c_str(),       //
         skyboxPath.c_str(),      //
         shaderDirectory.c_str(), //
         objectDirectory.c_str(), //
         stlDirectory.c_str());

#ifndef WEBAPP
  if (result.count("help")) {
    printf("%s", args.help({"", "Group"}).c_str());
    exit(0);
  }
#endif //  #ifndef WEBAPP

  return true;
}

int Options::LoadModel() {
  model.SetPath(modelPath);
  model.Load();
  scale = model.Scale();
  return 0;
}

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
                const vector<std::string> &filter) {
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

static std::vector<std::string> stlFilter = {".stl"};
static std::vector<std::string> objFilter = {".obj", ".dae", ".gltf"};

void Options::DrawGui() {
  ImFont *font = ImGui::GetFont();
  float fontSize = font->FontSize;

  if (ImGui::Button("Apply Model",
                    ImVec2(fontSize * 12, fontSize + fontSize / 5))) {
    model.Reload(modelPath);
  }
  ImGui::SameLine();
  ImGui::Text("%s", modelPath.stem().c_str());
  ImGui::Indent();
  if (ImGui::CollapsingHeader("Select##model", 0)) {
    ImGui::Indent();
    if (ImGui::CollapsingHeader("STL Drawings", 0)) {
      SelectFile(stlDirectory, modelPath, stlFilter);
    }
    if (ImGui::CollapsingHeader("Objects")) {
      SelectFile(objectDirectory, modelPath, objFilter);
    }
    ImGui::Unindent();
  }
  ImGui::Unindent();

  if (ImGui::Button("Apply Skybox",
                    ImVec2(fontSize * 12, fontSize + fontSize / 5))) {
    skyboxTexture = LoadCubemap(skyboxPath);
  }
  ImGui::SameLine();
  ImGui::Text("%s", skyboxPath.stem().c_str());
  ImGui::Indent();
  if (ImGui::CollapsingHeader("Select##skybox", 0)) {
    SelectFolder(skyboxDirectory, skyboxPath);
  }
  ImGui::Unindent();
}

float Options::skyboxVertices[] = {
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
size_t Options::skyboxVerticesSize = sizeof(skyboxVertices);

} // namespace viewgl
