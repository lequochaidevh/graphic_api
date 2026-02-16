#include "TestDynamicGeometry.h"

#include "VertexBufferLayout.h"

#include <array>

std::array<VerticesFp, 4> CreateQuadFp(float x, float y, float texIndex,
                                       float _width = 200) {
    float length = _width;  // 0 1 2 0 2 3

    VerticesFp v0;  // bottom left
    v0.position   = {x, y, 0.0f};
    v0.color      = {0.28f, 0.6f, 0.26f};
    v0.textCoords = {0.0f, 0.0f};
    v0.texIndex   = texIndex;
    VerticesFp v1 = v0;  // bottom right
    v1.position   = {x + length, y, 0.0f};
    v1.textCoords = {1.0f, 0.0f};
    VerticesFp v2 = v0;  // top right
    v2.position   = {x + length, y + length, 0.0f};
    v2.textCoords = {1.0f, 1.0f};
    VerticesFp v3 = v0;  // top left
    v3.position   = {x, y + length, 0.0f};
    v3.textCoords = {0.0f, 1.0f};

    return {v0, v1, v2, v3};
}

namespace test {
TestDynamicGeometry::TestDynamicGeometry()
    : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
      m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
      m_Translation(0.0f, 0.0f, 0.0f)  //
{
    // 0. Set up define and local variable
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    // NEW
    GLCall(glFrontFace(GL_CCW));
    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));

    // unsigned int indices[] = {0, 1, 3, 1, 2, 3, 4, 5, 7, 5, 6, 7};
    unsigned int indices[] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
    // 1. Init vertex buffer object // 1000 vertices
    m_VBO = std::make_unique<VertexBuffer>(nullptr, sizeof(VerticesFp) * 1000,
                                           "dynamic");

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
}

TestDynamicGeometry::~TestDynamicGeometry() {}

void TestDynamicGeometry::OnUpdate(float deltaTime) {
    // Demo: test dynamic buffer subribe
    // float vertices[] = {
    //     // position(3)      // color(3)     // texCoord(2)  // index (1)
    //     250.0f, 250.0f, 0.0f, 0.28f, 0.6f, 0.26f, 1.0f, 1.0f, 0.0f,  // t-r
    //     250.0f, -50.0f, 0.0f, 0.28f, 0.6f, 0.26f, 1.0f, 0.0f, 0.0f,  // b-r
    //     -50.0f, -50.0f, 0.0f, 0.28f, 0.6f, 0.26f, 0.0f, 0.0f, 0.0f,  // b-l
    //     -50.0f, 250.0f, 0.0f, 0.28f, 0.6f, 0.26f, 0.0f, 1.0f, 0.0f,  // t-l
    //                                                                  //
    //     400.0f, 400.0f, 0.0f, 1.0f, 0.88f, 0.66f, 1.0f, 1.0f, 1.0f,  // t-r
    //     400.0f, 100.0f, 0.0f, 1.0f, 0.88f, 0.66f, 1.0f, 0.0f, 1.0f,  // b-r
    //     100.0f, 100.0f, 0.0f, 1.0f, 0.88f, 0.66f, 0.0f, 0.0f, 1.0f,  // b-l
    //     100.0f, 400.0f, 0.0f, 1.0f, 0.88f, 0.66f, 0.0f, 1.0f, 1.0f   // t-l
    // }; - outdate position

    // NEW
    VerticesFp vertices[8];
    auto       q0 = CreateQuadFp(m_QuadPositionA[0], m_QuadPositionA[1], 0.0f);
    auto       q1 = CreateQuadFp(m_QuadPositionB[0], m_QuadPositionB[1], 1.0f);

    memcpy(vertices, q0.data(), q0.size() * sizeof(VerticesFp));
    // Note: size(byte) = 4 * VerticesFp
    memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(VerticesFp));

    m_VBO->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}

void TestDynamicGeometry::OnRender() {
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

void TestDynamicGeometry::OnImGuiRender() {
    ImGui::SliderFloat("Translation X", &m_Translation.x, 0.0f, 960.0f);
    ImGui::SliderFloat("Translation Y", &m_Translation.y, 0.0f, 540.0f);
    ImGui::DragFloat2("Quad Position A", m_QuadPositionA, 5.0f);
    ImGui::DragFloat2("Quad Position B", m_QuadPositionB, 5.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

};  // namespace test
