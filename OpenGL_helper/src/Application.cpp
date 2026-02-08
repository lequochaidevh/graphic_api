// ./cmakeBuild_and_Run.sh -p src/Application.cpp -c

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <filesystem>
#include <cmath>
#include <thread>
#include <chrono>

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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

    GLFWwindow *window = glfwCreateWindow(960, 540, "LearnOpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLCall(glfwSwapInterval(500));

    if (glewInit() != GLEW_OK) {
        std::cout << "Init Glew Failed\n";
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    glm::mat4 proj =                            // window sreen size
        glm::ortho(0.0f, 960.0f, 0.0f, 540.0f,  //
                   -1.0f, 1.0f);                // calib width:height 4:3

    glm::mat4 view = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(500.0f, 0.0f,
                  0.0f));  // move camera to left object will move right

    glm::mat4 model = glm::translate(
        glm::mat4(1.0f), glm::vec3(-300.0f, 0.0f, 0.0f));  // move model left

    glm::mat4 mvp = proj * view * model;  // model view projection

    Shader shader("res/shaders/BasicTexture.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
    shader.SetUniformMat4f("u_MVP", mvp);

    Texture texture("res/textures/C_nobgr.png");
    texture.Bind();                       // slot = 0 default
    shader.SetUniform1i("u_Texture", 0);  // slot = 0

    float vertices[] = {
        // position(3)      // color(3)     // texCoord(2)
        500.0f, 100.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
        500.0f, 500.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        100.0f, 500.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
        100.0f, 100.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };  // position not nomallize // real world position

    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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

    static float colorValue = 0;
    Renderer     renderer;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime    = currentFrame - lastFrame;
        lastFrame          = currentFrame;
        renderer.Clear(colorValue);
        renderer.Draw(VAO, EBO, shader);
        if (r > 1.0f)
            increase = -0.01f;
        else if (r < 0.01f)
            increase = 0.01f;

        r += blinkSpeed * deltaTime;
        colorValue = (std::sin(r) + 1.0f) / 2.0f;  // Smooth pulse 0.0 to 1.0

        shader.Bind();
        shader.SetUniform4f("u_Color", 0.3f, 0.8f, 1.0f - colorValue, 1.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
        printGPUStats();
        std::this_thread::sleep_for(std::chrono::milliseconds(66));  // 15fps
    }

    glfwTerminate();

    std::cout << "End main" << std::endl;
    return 0;
}