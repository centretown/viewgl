#pragma once

#include <glad.h>
#include <string>

struct TextureOptions {
  // GL_NEAREST: results in blocked patterns where we can clearly see the pixels
  // that form the texture while
  // GL_LINEAR: produces a smoother pattern where
  // the individual pixels are less visible.
  // GL_LINEAR_MIPMAP_LINEAR
  unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR;
  unsigned int magFilter = GL_LINEAR;
  // GL_REPEAT: The default behavior for textures. Repeats the texture image.
  // GL_MIRRORED_REPEAT: Same as GL_REPEAT but mirrors the image with each
  // repeat. GL_CLAMP_TO_EDGE: Clamps the coordinates between 0 and 1. The
  // result is that higher coordinates become clamped to the edge, resulting in
  // a stretched edge pattern. GL_CLAMP_TO_BORDER: Coordinates outside the range
  // are now given a user-specified border color.
  unsigned int wrapS = GL_REPEAT;
  unsigned int wrapT = GL_REPEAT;
  bool flip = false;
};

unsigned int MakeTexture(const char *filename, TextureOptions *options);
unsigned int LoadCubemap(std::string cubeDirectory);
