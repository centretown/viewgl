#include "input_options.hpp"

#include <cstdio>
#include <string>

const std::string resDir = "../resources/";
const std::string skyboxDir = "textures/skybox";
const std::string objDir = "objects";
const std::string stlDir = "stl";

int InputOptions::Parse(const char *title, int argc, const char **argv,
                        bool log) {
  std::string modelDir = stlDir;
  std::string modelName = "sphereofthedark.stl";
  std::string skyboxName = "islands";

  std::unique_ptr<cxxopts::Options> allocated(
      new cxxopts::Options(argv[0], title));

  cxxopts::Options &options = *allocated;
  options.set_width(150)
      .set_tab_expansion()
      .allow_unrecognised_options()
      .add_options() //
      ("d,directory", "Model Directory", cxxopts::value<std::string>(),
       "stl") //

      ("m,model", "Model File Name eg. 'enao.stl'",
       cxxopts::value<std::string>(),
       "sphereofthedark") //

      ("s,skybox", "Skybox Directory eg. 'islands'",
       cxxopts::value<std::string>(),
       "islands") //

      ("h,help", "Print help"); //

  cxxopts::ParseResult result = options.parse(argc, argv);

  if (result.count("d")) {
    modelDir = result["d"].as<std::string>().c_str();
  }

  if (result.count("m")) {
    modelName = result["m"].as<std::string>();
  }

  if (result.count("s")) {
    skyboxName = result["s"].as<std::string>();
  }

  if (result.count("help")) {
    printf("%s", options.help({"", "Group"}).c_str());
    printf("directory='%s' model='%s' skybox='%s'\n\n", modelDir.c_str(),
           modelName.c_str(), skyboxName.c_str());
    exit(0);
  }

  if (log)
    fprintf(stderr, "directory='%s' model='%s' skybox='%s'\n", modelDir.c_str(),
            modelName.c_str(), skyboxName.c_str());

  modelPath = resDir + modelDir + "/" + modelName;
  skyboxPath = resDir + skyboxDir + "/" + skyboxName;

  return true;
}
