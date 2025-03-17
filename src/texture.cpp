#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "texture.hpp"

#include <vector>

static GLenum GLformat(int nrChannels) {
  return (nrChannels == 3) ? GL_RGB : (nrChannels == 4) ? GL_RGBA : GL_RED;
}

unsigned int MakeTexture(const char *filename, TextureOptions *options) {
  unsigned int texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  stbi_set_flip_vertically_on_load(options->flip);
  int width, height, nrChannels;
  unsigned char *imageData =
      stbi_load(filename, &width, &height, &nrChannels, 0);
  if (!imageData) {
    stbi_image_free(imageData);
    printf("failed to load image data %s.\n", filename);
    return texture;
  }

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, options->wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, options->wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, options->minFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, options->magFilter);

  GLenum format = GLformat(nrChannels);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
               GL_UNSIGNED_BYTE, imageData);
  glGenerateMipmap(GL_TEXTURE_2D);

  printf("loaded texture %u from %s width=%d, height=%d, nrChannels=%d\n",
         texture, filename, width, height, nrChannels);
  stbi_image_free(imageData);

  return texture;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------

unsigned int LoadCubemap(std::string cubeDirectory) {
  printf("LoadCubemap skyboxPath=\"%s\"\n", cubeDirectory.c_str());
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  std::vector<std::string> faces = {
      std::string("/right"),  std::string("/left"),  std::string("/top"),
      std::string("/bottom"), std::string("/front"), std::string("/back")};

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    std::string f = cubeDirectory + faces[i] + ".png";
    unsigned char *data = stbi_load(f.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
      f = cubeDirectory + faces[i] + ".jpg";
      data = stbi_load(f.c_str(), &width, &height, &nrChannels, 0);
    }

    if (!data) {
      fprintf(stderr, "Cubemap texture failed to load at path: %s\n",
              f.c_str());
      continue;
    }

    GLenum format = GLformat(nrChannels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height,
                 0, format, GL_UNSIGNED_BYTE, data);
    fprintf(stderr, "Cubemap texture loaded at path: %s\n", f.c_str());
    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}
