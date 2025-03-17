#pragma once

#include <glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <stddef.h>

enum ShaderErrorCode {
  SHADER_VALID,
  SHADER_INVALID,
  SHADER_ERR_FILE,
  SHADER_ERR_COMPILE_VERTEX,
  SHADER_ERR_COMPILE_FRAGMENT,
  SHADER_ERR_LINK_PROGRAM,
};

#define SHADER_BUILD_FAIL 0
#define SHADER_BUILD_OK 1

struct Shader {
  const char *vertexPath = NULL;
  const char *fragmentPath = NULL;
  std::unique_ptr<char[]> vertexCode;
  std::unique_ptr<char[]> fragmentCode;
  // the program ID
  unsigned int ProgramID = (unsigned int)-1;
  ShaderErrorCode status = SHADER_INVALID;

  // constructor reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath)
      : vertexPath{vertexPath}, fragmentPath{fragmentPath} {}

  bool IsValid() { return status == SHADER_VALID; }
  int Build();
  // use/activate the shader
  void use() { glUseProgram(ProgramID); }
  // utility uniform functions
  void setBool(const char *name, bool value) const {
    glUniform1i(glGetUniformLocation(ProgramID, name), (int)value);
  }
  void setInt(const char *name, int value) const {
    glUniform1i(glGetUniformLocation(ProgramID, name), value);
  }
  void setFloat(const char *name, float value) const {
    glUniform1f(glGetUniformLocation(ProgramID, name), value);
  }
  // ------------------------------------------------------------------------
  void setVec2(const char *name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ProgramID, name), 1, &value[0]);
  }
  void setVec2(const char *name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ProgramID, name), x, y);
  }
  // ------------------------------------------------------------------------
  void setVec3(const char *name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ProgramID, name), 1, &value[0]);
  }
  void setVec3(const char *name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ProgramID, name), x, y, z);
  }
  // ------------------------------------------------------------------------
  void setVec4(const char *name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ProgramID, name), 1, &value[0]);
  }
  void setVec4(const char *name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ProgramID, name), x, y, z, w);
  }
  // ------------------------------------------------------------------------
  void setMat2(const char *name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ProgramID, name), 1, GL_FALSE,
                       &mat[0][0]);
  }
  // ------------------------------------------------------------------------
  void setMat3(const char *name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ProgramID, name), 1, GL_FALSE,
                       &mat[0][0]);
  }
  // ------------------------------------------------------------------------
  void setMat4(const char *name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ProgramID, name), 1, GL_FALSE,
                       &mat[0][0]);
  }

private:
  int Load();
  int LoadFile(const char *path, long &size, std::unique_ptr<char[]> &uptr);
};
