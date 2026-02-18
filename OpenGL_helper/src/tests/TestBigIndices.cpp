// clone by TestDynamicGeometry
#include "TestBigIndices.h"

#include "VertexBufferLayout.h"

#include <array>

static VerticesFp* CreateQuadFp_BigIndices(VerticesFp* target, float x, float y,
                                           float texIndex, float _width = 60) {
    float length = _width;  // 0 1 2 0 2 3

    // bottom left
    target->position   = {x, y, 0.0f};
    target->color      = {0.28f, 0.6f, 0.26f};
    target->textCoords = {0.0f, 0.0f};
    target->texIndex   = texIndex;
    target++;

    // bottom right
    target->position   = {x + length, y, 0.0f};
    target->color      = {0.28f, 0.6f, 0.26f};
    target->textCoords = {1.0f, 0.0f};
    target->texIndex   = texIndex;
    target++;

    // top right
    target->position   = {x + length, y + length, 0.0f};
    target->color      = {0.28f, 0.6f, 0.26f};
    target->textCoords = {1.0f, 1.0f};
    target->texIndex   = texIndex;
    target++;

    // top left
    target->position   = {x, y + length, 0.0f};
    target->color      = {0.28f, 0.6f, 0.26f};
    target->textCoords = {0.0f, 1.0f};
    target->texIndex   = texIndex;
    target++;

    return target;
}

namespace test {
TestBigIndices::TestBigIndices()
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

    // 1.01. Init indices array
    // unsigned int indices[] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
    std::vector<unsigned int> indices(m_MaxIndicesCount);
    unsigned int              ui32_Offset = 0;

    unsigned int QuadOrdering[6] = {
        0, 1, 2, 2, 3, 0,
    };  // CWW

    for (size_t i = 0; i < m_MaxIndicesCount;
         i += m_PointsPerQuad[QuadMetrics::Indices]) {
        for (size_t j = 0; j < m_PointsPerQuad[QuadMetrics::Indices]; j++) {
            indices[i + j] = QuadOrdering[j] + ui32_Offset;
        }
        ui32_Offset += m_PointsPerQuad[QuadMetrics::Vertices];
    }

    // 1. Init vertex buffer object // 1000 vertices
    m_VBO = std::make_unique<VertexBuffer>(
        nullptr, sizeof(VerticesFp) * m_MaxQuadCount, "dynamic");

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

    m_EBO = std::make_unique<IndexBuffer>(indices.data(),
                                          m_MaxIndicesCount);  // N elements

    // 4. Build Texture
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
    m_Texture_C_nobgr = std::make_unique<Texture>("res/textures/C_nobgr.png");
    m_Texture_H_nobgr = std::make_unique<Texture>("res/textures/H_nobgr.png");
}

TestBigIndices::~TestBigIndices() {}

void TestBigIndices::OnUpdate(float deltaTime) {
    // NEW: TODO Remake function build array
    uint32_t indexCount;

    std::array<VerticesFp, 1000> vertices;
    VerticesFp*                  buffer = vertices.data();
    // Build caro table with size 5 x 5
    for (size_t y = 0; y < 5; y++) {
        for (size_t x = 0; x < 5; x++) {
            buffer =
                CreateQuadFp_BigIndices(buffer, x * 50, y * 50, (x + y) % 2);
            indexCount += 6;
        }
    }

    buffer = CreateQuadFp_BigIndices(buffer, m_QuadPositionA[0],
                                     m_QuadPositionA[1], 0.0f, 200.0f);
    indexCount += 6;

    buffer = CreateQuadFp_BigIndices(buffer, m_QuadPositionB[0],
                                     m_QuadPositionB[1], 1.0f, 200.0f);
    indexCount += 6;

    m_VBO->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices.data());
}

void TestBigIndices::OnRender() {
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

void TestBigIndices::OnImGuiRender() {
    ImGui::SliderFloat("Translation X", &m_Translation.x, 0.0f, 960.0f);
    ImGui::SliderFloat("Translation Y", &m_Translation.y, 0.0f, 540.0f);
    ImGui::DragFloat2("Quad Position A", m_QuadPositionA, 5.0f);
    ImGui::DragFloat2("Quad Position B", m_QuadPositionB, 5.0f);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
};  // namespace test
