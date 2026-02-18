#pragma once
// clone by TestDynamicGeometry

#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"

#include "_Vert_Structure.h"

namespace test {
class TestBigIndices : public Test {
 public:
    TestBigIndices();
    ~TestBigIndices();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

 private:
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexArray>  m_VAO;
    std::unique_ptr<IndexBuffer>  m_EBO;
    std::unique_ptr<Shader>       m_Shader;
    std::unique_ptr<Texture>      m_Texture_C_nobgr;
    std::unique_ptr<Texture>      m_Texture_H_nobgr;

    glm::vec3 m_Translation;
    glm::mat4 m_Proj, m_View;

    float m_QuadPositionA[2] = {-50.0f, 250.0f};
    float m_QuadPositionB[2] = {100.0f, 300.0f};

    // 0.01. Define max vertices and indices for using
    struct QuadMetrics {
        enum Type : size_t { Vertices = 0, Indices = 1 };
    };
    const size_t m_PointsPerQuad[2] = {4, 6};
    const size_t m_MaxQuadCount     = 1000;
    // 0.02. build
    const size_t m_MaxVerticesCount =
        m_MaxQuadCount * m_PointsPerQuad[QuadMetrics::Vertices];
    const size_t m_MaxIndicesCount =
        m_MaxQuadCount * m_PointsPerQuad[QuadMetrics::Indices];
};
}  // namespace test
