#include "TestTextureRainbow.h"

#include "VertexBufferLayout.h"

namespace test {
TestTextureRainbow::TestTextureRainbow()
    : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
      m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
      m_Translation(50.0f, 50.0f, 0.0f),
      m_ChangeColor(0.1f, 0.1f, 0.1f)  //
{
    // 0. Set up define and local variable
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    float vertices[] = {
        // position(3)      // color(3)     // texCoord(2)
        250.0f, 250.0f, 0.0f, 0.28f, 0.6f, 0.26f, 1.0f, 1.0f,  // top right
        250.0f, -50.0f, 0.0f, 0.28f, 0.6f, 0.26f, 1.0f, 0.0f,  // bottom right
        -50.0f, -50.0f, 0.0f, 0.28f, 0.6f, 0.26f, 0.0f, 0.0f,  // bottom left
        -50.0f, 250.0f, 0.0f, 0.28f, 0.6f, 0.26f, 0.0f, 1.0f,  // top left
                                                               //
        400.0f, 400.0f, 0.0f, 1.0f, 0.88f, 0.66f, 1.0f, 1.0f,  // top right
        400.0f, 100.0f, 0.0f, 1.0f, 0.88f, 0.66f, 1.0f, 0.0f,  // bottom right
        100.0f, 100.0f, 0.0f, 1.0f, 0.88f, 0.66f, 0.0f, 0.0f,  // bottom left
        100.0f, 400.0f, 0.0f, 1.0f, 0.88f, 0.66f, 0.0f, 1.0f   // top left
    };  // position not nomallize // real world position

    unsigned int indices[] = {0, 1, 3, 1, 2, 3, 4, 5, 7, 5, 6, 7};

    // 1. Init vertex buffer object
    m_VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));

    // 2. Init path shader program
    m_Shader = Shader::FromGLSLTextFiles("res/shaders/vert.glsl.shader",
                                         "res/shaders/frag.glsl.shader");

    // 3. Build Vertex Array
    m_VAO = std::make_unique<VertexArray>();
    // local
    VertexBufferLayout layout;
    layout.Push<float>(3);  // POSITION
    layout.Push<float>(3);  // COLOR
    layout.Push<float>(2);  // TEXCOORD
    // push to instance
    m_VAO->AddBuffer(*m_VBO, layout);

    m_EBO = std::make_unique<IndexBuffer>(
        indices, sizeof(indices) / sizeof(unsigned int));  // 12 elements

    // 4. Build Texture
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
    m_Texture = std::make_unique<Texture>("res/textures/C_nobgr.png");
    m_Shader->SetUniform1i("u_Texture", 0);
    m_Shader->SetUniform1f("u_Time", 0);
}

TestTextureRainbow::~TestTextureRainbow() {}

void TestTextureRainbow::OnUpdate(float deltaTime) {}

void TestTextureRainbow::OnRender() {
    GLCall(glClearColor(0, 0, 0, 1));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;
    m_Texture->Bind();

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvp   = m_Proj * m_View * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
    }

    {
        m_Shader->SetUniform4f("u_Color", m_ChangeColor.r, m_ChangeColor.g,
                               m_ChangeColor.b, 1.0f);
    }

    static float  uTimeValue  = 0.0f;
    static float  step        = 0.1f;
    static double lastUpdate  = 0.0;
    double        currentTime = glfwGetTime();

    if (currentTime - lastUpdate >= 0.3) {
        uTimeValue += step;
        lastUpdate = currentTime;
    }

    if (uTimeValue >= 1.0f || uTimeValue <= 0.0f) {
        step = -step;  // reset to 0
        uTimeValue += step;
    }

    m_Shader->SetUniform1f("u_Time", uTimeValue);
    renderer.Draw(*m_VAO, *m_EBO, *m_Shader);
}

void TestTextureRainbow::OnImGuiRender() {
    ImGui::SliderFloat("Translation X", &m_Translation.x, 0.0f, 960.0f);
    ImGui::SliderFloat("Translation Y", &m_Translation.y, 0.0f, 540.0f);
    ImGui::SliderFloat3("ChangeColor", &m_ChangeColor.r, 0.0f, 1.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

};  // namespace test
