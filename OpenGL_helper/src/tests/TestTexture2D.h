#pragma once

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

namespace test {
class TestTexture2D : public Test {
 public:
    TestTexture2D();
    ~TestTexture2D();

    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnImGuiRender() override;

 private:
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<VertexArray>  m_VAO;
    std::unique_ptr<IndexBuffer>  m_EBO;
    std::unique_ptr<Shader>       m_Shader;
    std::unique_ptr<Texture>      m_Texture;

    glm::vec3 m_TranslationA, m_TranslationB;
    glm::mat4 m_Proj, m_View;
};
}  // namespace test
