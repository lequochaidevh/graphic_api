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
#include "tests/TestBigIndices.h"

#include "nvdia/NVDIA_Debugger.h"
#include <iostream>

#include "_Logger.h"
#include "_Logger_Core.h"

int main() {
    std::thread t1([] { LOG_INFO("THREAD Worker running"); });
    t1.join();
    int a = 3, b = 4;
    LOG_SET_FILE_STORE(LOGER_FILE_DEFAULT_PATH);
    LOG_TRACE("ABC {} {}", a, b);
    LOG_DEBUG("ABC {} {}", a, b);
    LOG_INFO("ABC {} {}", a, b);
    LOG_WARN("ABC {} {}", a, b);
    LOG_ERROR("ABC {} {}", a, b);
    LOG_CRITICAL("ABC {} {}", a, b);

    LOG_SET_BACKEND;  // set backend is AsyncLogger
    async_log_trace("ABC {} {}", a, b);
    async_log_debug("ABC {} {}", a, b);
    async_log_info("ABC {} {}", a, b);
    async_log_warn("ABC {} {}", a, b);
    async_log_error("ABC {} {}", a, b);
    async_log_critical("ABC {} {}", a, b);
    LOG_BACKEND_STOP;  // sync queue // critical safety TODO handle if not
                       // called

    std::thread t([] { LOG_INFO("THREAD Worker running"); });
    t.join();

    CORE_LOG_SET_FILE_STORE(CORE_LOGER_FILE_DEFAULT_PATH);
    CORE_LOG_TRACE("CORE {} {}", a, b);
    CORE_LOG_DEBUG("CORE {} {}", a, b);
    CORE_LOG_INFO("CORE {} {}", a, b);
    CORE_LOG_WARN("CORE {} {}", a, b);
    CORE_LOG_ERROR("CORE {} {}", a, b);
    CORE_LOG_CRITICAL("CORE {} {}", a, b);

    CORE_LOG_SET_BACKEND;
    async_core_log_trace("CORE ABC {} {}", a, b);
    async_core_log_debug("CORE ABC {} {}", a, b);
    async_core_log_info("CORE ABC {} {}", a, b);
    async_core_log_warn("CORE ABC {} {}", a, b);
    async_core_log_error("CORE ABC {} {}", a, b);
    async_core_log_critical("CORE ABC {} {}", a, b);
    CORE_LOG_BACKEND_STOP;  // critical safety

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
    testMenuPtr->RegisterTest<test::TestBigIndices>("Big Indices");
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