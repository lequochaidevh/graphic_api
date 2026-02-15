#include "TestTextureIndex.h"

#include "VertexBufferLayout.h"

namespace test {
TestTextureIndex::TestTextureIndex()
    : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
      m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
      m_Translation(50.0f, 50.0f, 0.0f)  //
{
    // 0. Set up define and local variable
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    float vertices[] = {
        // position(3)      // color(3)     // texCoord(2)  // index (1)
        250.0f, 250.0f, 0.0f, 0.28f, 0.6f, 0.26f, 1.0f, 1.0f, 0.0f,  // t-r
        250.0f, -50.0f, 0.0f, 0.28f, 0.6f, 0.26f, 1.0f, 0.0f, 0.0f,  // b-r
        -50.0f, -50.0f, 0.0f, 0.28f, 0.6f, 0.26f, 0.0f, 0.0f, 0.0f,  // b-l
        -50.0f, 250.0f, 0.0f, 0.28f, 0.6f, 0.26f, 0.0f, 1.0f, 0.0f,  // t-l
                                                                     //
        400.0f, 400.0f, 0.0f, 1.0f, 0.88f, 0.66f, 1.0f, 1.0f, 1.0f,  // t-r
        400.0f, 100.0f, 0.0f, 1.0f, 0.88f, 0.66f, 1.0f, 0.0f, 1.0f,  // b-r
        100.0f, 100.0f, 0.0f, 1.0f, 0.88f, 0.66f, 0.0f, 0.0f, 1.0f,  // b-l
        100.0f, 400.0f, 0.0f, 1.0f, 0.88f, 0.66f, 0.0f, 1.0f, 1.0f   // t-l
    };  // position not nomallize // real world position
    // top right
    // bottom right
    // bottom left
    // top left

    unsigned int indices[] = {0, 1, 3, 1, 2, 3, 4, 5, 7, 5, 6, 7};

    // 1. Init vertex buffer object
    m_VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));

    // 2. Init path shader program
    m_Shader = Shader::FromGLSLTextFiles("res/shaders/TextureIndexVert.shader",
                                         "res/shaders/TextureIndexFrag.shader");

    // NEW TODO
    m_Shader->Bind();
    int sampler[2] = {0, 1};
    m_Shader->SetUniform1iv("u_Textures", 2, sampler);
    // std::vector<int> samplers = {0, 1};
    // shader.SetUniform1iv("u_Textures", samplers.size(), samplers.data());

    // 3. Build Vertex Array
    m_VAO = std::make_unique<VertexArray>();
    // local
    VertexBufferLayout layout;
    layout.Push<float>(3);  // POSITION
    layout.Push<float>(3);  // COLOR
    layout.Push<float>(2);  // TEXCOORD
    layout.Push<float>(1);  // INDEX
    // push to instance
    m_VAO->AddBuffer(*m_VBO, layout);

    m_EBO = std::make_unique<IndexBuffer>(
        indices, sizeof(indices) / sizeof(unsigned int));  // 12 elements

    // 4. Build Texture
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
    m_Texture_C_nobgr = std::make_unique<Texture>("res/textures/C_nobgr.png");
    m_Texture_H_nobgr = std::make_unique<Texture>("res/textures/H_nobgr.png");
    // m_Shader->SetUniform1i("u_Texture", 0);  // slot = 0
}

TestTextureIndex::~TestTextureIndex() {}

void TestTextureIndex::OnUpdate(float deltaTime) {}

void TestTextureIndex::OnRender() {
    GLCall(glClearColor(0.4f, 0.4f, 0.3f, 1));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));

    Renderer renderer;
    m_Texture_C_nobgr->Bind();
    m_Texture_H_nobgr->Bind();

    // NEW TODO
    glBindTextureUnit(0, m_Texture_C_nobgr->GetRendererID());
    glBindTextureUnit(1, m_Texture_H_nobgr->GetRendererID());

    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
        glm::mat4 mvp   = m_Proj * m_View * model;
        m_Shader->SetUniformMat4f("u_MVP", mvp);
    }

    renderer.Draw(*m_VAO, *m_EBO, *m_Shader);
}

void TestTextureIndex::OnImGuiRender() {
    ImGui::SliderFloat("Translation X", &m_Translation.x, 0.0f, 960.0f);
    ImGui::SliderFloat("Translation Y", &m_Translation.y, 0.0f, 540.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

};  // namespace test
