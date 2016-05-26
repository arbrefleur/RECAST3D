#pragma once

#include <string>

#include <GL/gl3w.h>

namespace tomovis {

class ShaderProgram {
  public:
    ShaderProgram(std::string vert, std::string frag);
    ~ShaderProgram();

    void use();
    GLuint handle() { return shader_program_; }

  private:
    GLuint shader_program_;
    GLuint vert_shader_;
    GLuint frag_shader_;
};

} // namespace tomovis
