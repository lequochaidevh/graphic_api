#pragma once
#include <csignal>  // raise(SIGTRAP)
#include <iostream>

#if defined(__MSVC__)
#define BREAK_POINT __debugbreak()
#else
#define BREAK_POINT raise(SIGTRAP)
#endif

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#define ERR_HANDLING_ACTIVE true

#define ASSERT_GL(x) \
    if (!(x)) BREAK_POINT;

#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT_GL(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

class Renderer {
 private:
    /* data */
 public:
    Renderer(/* args */);
    ~Renderer();

    void Clear();
    void Clear(float r);

    void Draw(const VertexArray& va, const IndexBuffer& ib,
              const Shader& shader);
};
