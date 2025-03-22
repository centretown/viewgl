#include "options.hpp"

#include <cstdio>
#include <string>

namespace viewgl {

int Options::Parse(const char *title, int argc, const char **argv, bool log) {

  std::unique_ptr<cxxopts::Options> allocated(
      new cxxopts::Options(argv[0], title));

  cxxopts::Options &options = *allocated;
  options.set_width(150)
      .set_tab_expansion()
      .allow_unrecognised_options()
      .add_options() //
      ("r,resources", "Resource Directory",
       cxxopts::value<std::string>(), //
       resourceDir)                   //

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

  cxxopts::ParseResult result = options.parse(argc, argv);

  if (result.count("r")) {
    resourceDir = result["r"].as<std::string>().c_str();
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

  std::string dir;
  if (modelType == "stl")
    dir = stlDir;
  else
    dir = modelDir + "/" + modelName;

  modelPath = resourceDir + "/" + dir + "/" + modelName + "." + modelType;

  skyboxPath = resourceDir;
  skyboxPath /= skyboxDir;
  skyboxPath /= skyboxName;

  shaderPath = resourceDir;
  shaderPath /= shaderDir;

  objectPath = resourceDir + "/" + modelDir;
  stlPath = resourceDir + "/" + stlDir;
  skyboxPaths = resourceDir + "/" + skyboxDir;

  for (auto const &dir_entry :
       std::filesystem::directory_iterator{resourceDir}) {
    if (log)
      fprintf(stderr, "%s\n", dir_entry.path().c_str());
  }

  FillVector(shaderPath, shaderList);
  FillVector(skyboxPaths, skyboxList);
  FillVector(objectPath, objectList);
  FillVector(stlPath, stlList);

  if (log)
    fprintf(stderr,
            "resourcePath='%s'\n"
            "modelPath='%s'\n"
            "skyboxPath='%s'\n"
            "shaderPath='%s'\n"
            "objectPath='%s'\n"
            "stlPath='%s'\n",    //
            resourceDir.c_str(), //
            modelPath.c_str(),   //
            skyboxPath.c_str(),  //
            shaderPath.c_str(),  //
            objectPath.c_str(), stlPath.c_str());

  if (result.count("help")) {
    printf("%s", options.help({"", "Group"}).c_str());
    exit(0);
  }

  return true;
}

int Options::LoadModel() {
  model.SetPath(modelPath);
  model.Load();
  scale = model.Scale();
  return 0;
}

void FillVector(std::filesystem::path &dir,
                std::vector<std::filesystem::path> &list, bool log) {
  for (auto const &dir_entry : std::filesystem::directory_iterator{dir}) {
    if (log)
      fprintf(stderr, "%s\n", dir_entry.path().c_str());
    list.push_back(dir_entry);
  }
}
} // namespace viewgl
