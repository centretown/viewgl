#include "model.hpp"
#include "texture.hpp"

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <glm/detail/qualifier.hpp>

void Model::Draw(Shader &shader) {
  for (unsigned int i = 0; i < meshes.size(); i++)
    meshes[i].Draw(shader);
}

void Model::loadModel(string path) {
  Assimp::Importer import;
  const aiScene *scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    printf("ERROR::ASSIMP:: %s\n", import.GetErrorString());
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);

  if (meshes.size() == 1 && textures_loaded.size() == 0) {
    Mesh &mesh = meshes[0];
    // loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    TextureOptions options = {
        GL_LINEAR,
        GL_LINEAR,
        GL_REPEAT,
        GL_REPEAT,
    };
    string filename = "assets/textures/red.png";
    Texture texture = {0};
    texture.id = MakeTexture(filename.c_str(), &options);
    texture.type = "texture_diffuse";
    texture.path = filename;
    mesh.textures.push_back(texture);
    textures_loaded.push_back(texture);
  }

  printf("Model:\n\t# of meshes = %ld\n\t# of textures = %ld\n", meshes.size(),
         textures_loaded.size());
  printf("Bounding Box min={x=%.2f, y=%.2f, z=%.2f} max={x=%.2f, y=%.2f, "
         "z=%.2f}\n",
         min.x, min.y, min.z, max.x, max.y, max.z);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

void Model::MinMax(glm::vec3 vector) {
  if (vector.x < min.x)
    min.x = vector.x;
  if (vector.x > max.x)
    max.x = vector.x;
  if (vector.y < min.y)
    min.y = vector.y;
  if (vector.y > max.y)
    max.y = vector.y;
  if (vector.z < min.z)
    min.z = vector.z;
  if (vector.z > max.z)
    max.z = vector.z;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
  vector<Vertex> vertices;
  vector<unsigned int> indices;
  vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;

    MinMax(vector);

    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;

    // does the mesh contain texture coordinates?
    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    } else {
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);
    }
    vertices.push_back(vertex);
  }

  // process indices
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  // process material
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    vector<Texture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }
  return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            string typeName) {
  vector<Texture> textures;
  TextureOptions options = {
      GL_LINEAR_MIPMAP_LINEAR,
      GL_LINEAR,
      GL_REPEAT,
      GL_REPEAT,
  };

  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < textures_loaded.size(); j++) {
      if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(textures_loaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) { // if texture hasn't been loaded already, load it
      string filename = directory + '/' + str.C_Str();
      Texture texture = {0};
      texture.id = MakeTexture(filename.c_str(), &options);
      texture.type = typeName;
      texture.path = str.C_Str();
      textures.push_back(texture);
      textures_loaded.push_back(texture); // add to loaded textures
    }
  }
  return textures;
}
