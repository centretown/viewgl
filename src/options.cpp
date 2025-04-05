#include "options.hpp"
#include <cxxopts.hpp>

#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <string>

namespace viewgl {

// std::string resourceBase = "/home/dave/src/viewgl/run/resources/";
// std::string modelName = "sphereofthedark";
// std::string skyboxName = "islands";
// std::string modelType = "stl";

const std::string shaderDir = "shaders";
const std::string textureDir = "textures";
const std::string modelDir = "objects";
const std::string skyboxDir = "skybox";
const std::string stlDir = "stl";
const std::string subDir = "";

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

  objectDirectory = resourceBase + modelDir;
  stlDirectory = resourceBase + stlDir;
  skyboxDirectory = resourceBase + skyboxDir;

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
