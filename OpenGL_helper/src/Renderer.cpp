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

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0));
}
