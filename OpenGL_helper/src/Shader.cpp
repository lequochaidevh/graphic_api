#include "Shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <filesystem>
#include <cmath>

#include "Renderer.h"

Shader::Shader(const std::string &filepath)
    : m_Filepath(filepath), m_RendererID(0) {
    source       = ParseShader(m_Filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
    std::cout << "m_RendererID = " << m_RendererID << std::endl;
    ASSERT_GL(m_RendererID != 0 && "Failed to create Shader ID");
}

Shader::~Shader() { GLCall(glDeleteProgram(m_RendererID)); }

void Shader::Bind() const { GLCall(glUseProgram(m_RendererID)); }

void Shader::Unbind() const { GLCall(glUseProgram(0)); }

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2,
                          float v3) {
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform1f(const std::string &name, float value) {
    GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string &name, int value) {
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

int Shader::GetUniformLocation(const std::string &name) {
    Bind();  // safety

    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    ASSERT_GL(location != -1 && "Not found Uniform");

    m_UniformLocationCache[name] = location;

    return location;
}

ShaderProgramSource Shader::ParseShader(const std::string &filepath) {
    std::ifstream stream(filepath);
    if (!stream.is_open()) {
        std::cout << "Current path: " << std::filesystem::current_path()
                  << std::endl;
        std::cerr << "Can not found file at: " << filepath << std::endl;
    }
    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::string       line;
    std::stringstream ss[2];
    ShaderType        type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                // set mode to vertex
                type = ShaderType::VERTEX;
            } else if (line.find("fragment") != std::string::npos) {
                // setmode to fragment
                type = ShaderType::FRAGMENT;
            }
        } else {
            if (type != ShaderType::NONE) {
                ss[(int)type] << line << '\n';
            }
        }
    }

    return {ss[0].str(), ss[1].str()};
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source,
                                   bool errHandlingEnable = false) {
    unsigned int id  = glCreateShader(type);
    const char * src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, NULL));
    GLCall(glCompileShader(id));

    if (errHandlingEnable) {
        int  success;
        char infoLog[512];
        GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &success));

        if (!success) {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::_::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string &vertexShader,
                                  const std::string &fragmentShader) {
    // ===== Compile fragment shader =====
    unsigned int vshader =
        CompileShader(GL_VERTEX_SHADER, vertexShader, ERR_HANDLING_ACTIVE);
    unsigned int fshader =
        CompileShader(GL_FRAGMENT_SHADER, fragmentShader, ERR_HANDLING_ACTIVE);

    unsigned int shaderProgram = glCreateProgram();
    GLCall(glAttachShader(shaderProgram, vshader));
    GLCall(glAttachShader(shaderProgram, fshader));
    GLCall(glLinkProgram(shaderProgram));

    GLCall(glDeleteShader(vshader));
    GLCall(glDeleteShader(fshader));

    return shaderProgram;
}