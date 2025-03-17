#pragma once
#include <cxxopts.hpp>

struct InputOptions {
  std::string modelPath;
  std::string skyboxPath;
  int Parse(const char *title, int argc, const char **argv, bool log = false);
};
