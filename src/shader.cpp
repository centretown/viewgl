#include "shader.hpp"
#include <cerrno>
#include <memory>
#include <stdio.h>
#include <string.h>

namespace viewgl {

int Shader::LoadFile(const char *path, long &size,
                     std::unique_ptr<char[]> &unique) {
  // file_size( const std::filesystem::path& p )
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    printf("unable to open shader program. '%s' %s\n", path, strerror(errno));
    return SHADER_BUILD_FAIL;
  }

  // determine size
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *codeBuffer = new char[size + 1];
  fread(codeBuffer, 1, size, file);
  codeBuffer[size] = 0;
  unique.reset(codeBuffer);
  fclose(file);

  return SHADER_BUILD_OK;
}

int Shader::Load() {
  long vertexLength = 0;
  long fragmentLength = 0;
  if (!LoadFile(vertexPath, vertexLength, vertexCode)) {
    return SHADER_BUILD_FAIL;
  }
  printf("read %ld bytes from %s\n", vertexLength, vertexPath);
  if (!LoadFile(fragmentPath, fragmentLength, fragmentCode)) {
    return SHADER_BUILD_FAIL;
  }
  printf("read %ld bytes from %s\n", fragmentLength, fragmentPath);
  return SHADER_BUILD_OK;
}

int Shader::Build() {
  if (!Load()) {
    status = SHADER_ERR_FILE;
    return SHADER_BUILD_FAIL;
  }

  unsigned int vertex;
  unsigned int fragment;
  int success;
  char infoLog[2048];

  // compile shaders
  vertex = glCreateShader(GL_VERTEX_SHADER);
  char *ptr = vertexCode.get();
  glShaderSource(vertex, 1, &ptr, NULL);
  glCompileShader(vertex);

  // check for compile errors if any
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, sizeof(infoLog), NULL, infoLog);
    printf("'%s' Vertex Shader compilation failed\n%s\n", vertexPath, infoLog);
    status = SHADER_ERR_COMPILE_VERTEX;
    return SHADER_BUILD_FAIL;
  };

  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  ptr = fragmentCode.get();
  glShaderSource(fragment, 1, &ptr, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, sizeof(infoLog), NULL, infoLog);
    printf("'%s' Fragment Shader compilation failed\n%s\n", fragmentPath,
           infoLog);
    status = SHADER_ERR_COMPILE_FRAGMENT;
    return SHADER_BUILD_FAIL;
  }

  // shader Program
  ProgramID = glCreateProgram();
  glAttachShader(ProgramID, vertex);
  glAttachShader(ProgramID, fragment);
  glLinkProgram(ProgramID);
  // print linking errors if any
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ProgramID, sizeof(infoLog), NULL, infoLog);
    printf("'%s' '%s' Shader linkage failed\n%s\n", vertexPath, fragmentPath,
           infoLog);
    status = SHADER_ERR_LINK_PROGRAM;
    return SHADER_BUILD_FAIL;
  }

  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  status = SHADER_VALID;
  return SHADER_BUILD_OK;
}
} // namespace viewgl
