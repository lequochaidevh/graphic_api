#include "Renderer.h"

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

bool GLLogCall() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer() {}

Renderer::~Renderer() {}

void Renderer::Clear() { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib,
                    const Shader& shader) {
    shader.Bind();
    va.Bind();
    ib.Bind();

    // todo add material
    // shader.Bind();
    // shader.SetUniform4f("u_Color", colorValue, 0.3f, 0.8f, 1.0f);

    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}
