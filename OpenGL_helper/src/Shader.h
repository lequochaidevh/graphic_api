#pragma once
#include <string>
#include <unordered_map>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class Shader {
 private:
    std::string                                    m_Filepath;
    unsigned int                                   m_RendererID;
    mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;

 public:
    Shader(const std::string &filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1f(const std::string &name, float v0);
    void SetUniform2f(const std::string &name, float v0, float v1);
    void SetUniform3f(const std::string &name, float v0, float v1, float v2);
    void SetUniform4f(const std::string &name, float v0, float v1, float v2,
                      float v3);

    void SetUniform1i(const std::string &name, int v0);
    void SetUniform2i(const std::string &name, int v0, int v1);
    void SetUniform3i(const std::string &name, int v0, int v1, int v2);
    void SetUniform4i(const std::string &name, int v0, int v1, int v2, int v3);

    void SetUniformVec2f(const std::string &name, const glm::vec2 &v);
    void SetUniformVec3f(const std::string &name, const glm::vec3 &v);
    void SetUniformVec4f(const std::string &name, const glm::vec4 &v);

    void SetUniformMat3f(const std::string &name, const glm::mat3 &matrix);
    void SetUniformMat4f(const std::string &name, const glm::mat4 &matrix);

 private:
    GLint               GetUniformLocation(const std::string &name) const;
    ShaderProgramSource ParseShader(const std::string &filepath);
    ShaderProgramSource source;
    unsigned int CompileShader(unsigned int type, const std::string &source,
                               bool errHandlingEnable);
    unsigned int CreateShader(const std::string &vertexShader,
                              const std::string &fragmentShader);
};
