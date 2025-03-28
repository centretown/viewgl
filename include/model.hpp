#pragma once

#include "mesh.hpp"
#include "shader.hpp"

#include <assimp/scene.h> // Output data structure

namespace viewgl {

class Model {
  std::string path;
  bool gammaCorrection;
  vector<Texture> textures_loaded;
  vector<Mesh> meshes;
  const aiScene *scene = NULL;

public:
  glm::vec3 min = {0, 0, 0};
  glm::vec3 max = {0, 0, 0};
  string directory;

  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                       string typeName);

  void Load();
  void Draw(Shader &shader);
  float Scale();
  void MinMax(glm::vec3 vector);
  void dump();
  void Reload(string const &p, bool gamma = false);

public:
  Model() {}
  Model(string const &path, bool gamma = false)
      : path{path}, gammaCorrection(gamma) {}

  void SetPath(string const &p, bool gamma = false) {
    path = p;
    gammaCorrection = gamma;
  }

  static std::string resourceDirectory;
  static std::string GetResourceDirectory();
  static void SetResourceDirectory(std::string directory);
};
} // namespace viewgl
