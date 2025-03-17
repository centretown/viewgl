#pragma once

#include "mesh.hpp"
#include "shader.hpp"

#include <assimp/scene.h> // Output data structure

class Model {
  vector<Texture> textures_loaded;
  vector<Mesh> meshes;

public:
  glm::vec3 min = {0, 0, 0};
  glm::vec3 max = {0, 0, 0};
  string directory;
  bool gammaCorrection;

  void loadModel(string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                       string typeName);
  void MinMax(glm::vec3 vector);

public:
  Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
    loadModel(path);
  }
  void Draw(Shader &shader);
};
