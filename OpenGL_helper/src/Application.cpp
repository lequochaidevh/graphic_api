// ./cmakeBuild_and_Run.sh -p src/Application.cpp -c

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <filesystem>
#include <cmath>

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include <nvml.h>
#include <iostream>

void printGPUStats() {
    nvmlInit();  // Initialize NVML

    nvmlDevice_t device;
    // Get handle for the first GPU (index 0)
    nvmlDeviceGetHandleByIndex(0, &device);

    // 1. Get Temperature
    unsigned int temp;
    nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);

    // 2. Get Utilization (% GPU Used)
    nvmlUtilization_t utilization;
    nvmlDeviceGetUtilizationRates(device, &utilization);

    std::cout << "GPU Temp: " << temp << " C" << std::endl;
    std::cout << "GPU Usage: " << utilization.gpu << " %" << std::endl;

    nvmlShutdown();  // Clean up
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

    GLCall(glfwSwapInterval(10));

    if (glewInit() != GLEW_OK) {
        std::cout << "Init Glew Failed\n";
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    Shader shader("res/shaders/BasicTexture.shader");
    shader.Bind();

    Texture texture("res/textures/C_background.png");
    texture.Bind();                       // slot = 0 default
    shader.SetUniform1i("u_Texture", 0);  // slot = 0

    float vertices[] = {
        // position(3)      // color(3)     // texCoord(2)
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

    VertexBuffer VBO(vertices, sizeof(vertices));

    IndexBuffer EBO(indices,
                    sizeof(indices) / sizeof(unsigned int));  // 6 elements

    VertexArray        VAO;
    VertexBufferLayout layout;
    layout.Push<float>(3);  // POSITION
    layout.Push<float>(3);  // COLOR
    layout.Push<float>(2);  // TEXCOORD
    VAO.AddBuffer(VBO, layout);

    // Need Init EBO affter VAO
    EBO.Bind();  // safety CRITICALLL!!! 4. Bind EBO while VAO is still bound!

    // Now it is safe to unbind (opt)
    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();

    float r        = 0.0;
    float increase = 0.05;

    float lastFrame  = 0.0f;
    float blinkSpeed = 1.5f;  // Adjust this to change speed

    Renderer renderer;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime    = currentFrame - lastFrame;
        lastFrame          = currentFrame;
        renderer.Clear();
        renderer.Draw(VAO, EBO, shader);
        if (r > 1.0f)
            increase = -0.01f;
        else if (r < 0.01f)
            increase = 0.01f;

        r += blinkSpeed * deltaTime;
        float colorValue =
            (std::sin(r) + 1.0f) / 2.0f;  // Smooth pulse 0.0 to 1.0

        shader.Bind();
        // shader.SetUniform4f("u_Color", colorValue, 0.3f, 0.8f, 1.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
        printGPUStats();
    }

    glfwTerminate();

    std::cout << "End main" << std::endl;
    return 0;
}