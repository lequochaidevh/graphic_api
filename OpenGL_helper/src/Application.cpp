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
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestSandbox.h"
#include "tests/TestTextureRainbow.h"
#include "tests/TestTextureIndex.h"
#include "tests/TestDynamicGeometry.h"

#include "nvdia/NVDIA_Debugger.h"
#include <iostream>

#include "_Logger.h"

int main() {
    int a = 3, b = 4;
    LOG_TRACE("ABC {} {}", a, b);
    LOG_DEBUG("ABC {} {}", a, b);
    LOG_INFO("ABC {} {}", a, b);
    LOG_WARN("ABC {} {}", a, b);
    LOG_ERROR("ABC {} {}", a, b);
    LOG_CRITICAL("ABC {} {}", a, b);

    std::thread t([] { LOG_INFO("THREAD Worker running"); });
    t.join();

    pid_t child = fork();

    if (child < 0) {
        perror("fork failed");
        return 1;
    }

    if (child == 0) {
        // Child process
        LOG_INFO("=== CHILD ===");
        LOG_INFO("getpid {} ", getpid());
    } else {
        // Parent process
        LOG_INFO("=== PARENT ===");
        LOG_INFO("getpid {} CHILD_PID: {}", getpid(), child);

        wait(nullptr);  // wait child end
    }

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(960, 540, "LearnOpenGL", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    GLCall(glfwSwapInterval(1));

    if (glewInit() != GLEW_OK) {
        std::cout << "Init Glew Failed\n";
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    Renderer renderer;

    // IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

    test::Test*     currentTestPtr = nullptr;
    test::TestMenu* testMenuPtr    = new test::TestMenu(currentTestPtr);
    currentTestPtr                 = testMenuPtr;  // init
    testMenuPtr->RegisterTest<test::TestClearColor>("Clear Color");
    testMenuPtr->RegisterTest<test::TestTexture2D>("2D Texture");
    testMenuPtr->RegisterTest<test::TestSandbox>("Sandbox base");
    testMenuPtr->RegisterTest<test::TestTextureRainbow>("Texture Rainbow");
    testMenuPtr->RegisterTest<test::TestTextureIndex>("Texture Index");
    testMenuPtr->RegisterTest<test::TestDynamicGeometry>("Dynamic Geometry");
    while (!glfwWindowShouldClose(window)) {
        renderer.Clear();

        ImGui_ImplGlfwGL3_NewFrame();

        if (currentTestPtr) {
            currentTestPtr->OnUpdate(0.0f);
            currentTestPtr->OnRender();
            ImGui::Begin("Test");
            if (currentTestPtr != testMenuPtr && ImGui::Button("[<=]")) {
                std::cout << "Delete Constructor currentTest" << std::endl;
                delete currentTestPtr;
                currentTestPtr = testMenuPtr;
            }
            currentTestPtr->OnImGuiRender();
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));  // 15fps
    }

    // Close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    std::cout << "End main" << std::endl;
    return 0;
}