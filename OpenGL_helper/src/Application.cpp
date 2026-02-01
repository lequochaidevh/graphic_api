// ./cmakeBuild_and_Run.sh -p src/Application.cpp -c

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <filesystem>
#include <csignal>  // raise(SIGTRAP)

#if defined(__MSVC__)
#define BREAK_POINT __debugbreak()
#else
#define BREAK_POINT raise(SIGTRAP)
#endif

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#define ERR_HANDLING_ACTIVE true

#define ASSERT__(x) \
    if (!(x)) BREAK_POINT;

#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT__(GLLogCall())

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

static bool GLLogCall() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string &filepath) {
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

static unsigned int CompileShader(unsigned int type, const std::string &source,
                                  bool errHandlingEnable = false) {
    unsigned int id  = glCreateShader(type);
    const char * src = source.c_str();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);

    if (errHandlingEnable) {
        int  success;
        char infoLog[512];
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::_::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    }

    return id;
}

static unsigned int CreateShader(const std::string &vertexShader,
                                 const std::string &fragmentShader) {
    // ===== Compile fragment shader =====
    unsigned int vshader =
        CompileShader(GL_VERTEX_SHADER, vertexShader, ERR_HANDLING_ACTIVE);
    unsigned int fshader =
        CompileShader(GL_FRAGMENT_SHADER, fragmentShader, ERR_HANDLING_ACTIVE);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vshader);
    glAttachShader(shaderProgram, fshader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return shaderProgram;
}

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "Init Glew Failed\n";
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "VERTEX source:\n " << source.VertexSource << std::endl;
    std::cout << "FRAGMENT source:\n " << source.FragmentSource << std::endl;

    unsigned int shaderProgram =
        CreateShader(source.VertexSource, source.FragmentSource);

    float vertices[] = {
        // position(3)      // color(3)     // texCoord(2)
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

    // each vertex = 8 float
    // 3 pos + 3 color + 2 tex = 8
    // stride = 8 * sizeof(float)

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // ===== ATTRIBUTE 0 : POSITION =====
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // ===== ATTRIBUTE 1 : COLOR =====
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // ===== ATTRIBUTE 2 : TEXCOORD =====
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // wireframe mode
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // default

        // GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_INT, 0));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
}