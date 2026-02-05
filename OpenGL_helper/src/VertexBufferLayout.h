#pragma once
#include <vector>
#include <cassert>
#include "Renderer.h"

struct VertexBufferElement {
    unsigned int  type;
    unsigned int  count;
    unsigned char normalized;

    static unsigned int GetSizeOfType(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
                return sizeof(float);
            case GL_UNSIGNED_INT:
                return sizeof(unsigned int);
            case GL_UNSIGNED_BYTE:
                return sizeof(char);
            default:
                break;
        }
        ASSERT_GL(false);
        return 0;
    }
};

class VertexBufferLayout {
 private:
    std::vector<VertexBufferElement> m_Elements;
    unsigned int                     m_Stride;

 public:
    VertexBufferLayout() : m_Stride(0) {}
    ~VertexBufferLayout();

    template <typename T>
    void Push(unsigned int count);

    // template <>
    // void Push<float>(unsigned int count) {
    //     m_Elements.push_back({GL_FLOAT, count, false});
    //     m_Stride += sizeof(GLfloat);
    // }

    // template <>
    // void Push<unsigned int>(unsigned int count) {
    //     m_Elements.push_back({GL_UNSIGNED_INT, count, false});
    //     m_Stride += sizeof(GLuint);
    // }

    // template <>
    // void Push<unsigned char>(unsigned int count) {
    //     m_Elements.push_back({GL_UNSIGNED_BYTE, count, false});
    //     m_Stride += sizeof(GLubyte);
    // }

    inline std::vector<VertexBufferElement> GetElement() const {
        return m_Elements;
    }

    inline unsigned int GetStride() const { return m_Stride; }
};

template <typename T>
void VertexBufferLayout::Push(unsigned int count) {
    static_assert(sizeof(T) == 0, "VertexBufferLayout::Push: unsupported type");
}

template <>
inline void VertexBufferLayout::Push<float>(unsigned int count) {
    m_Elements.push_back({GL_FLOAT, count, GL_FALSE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template <>
inline void VertexBufferLayout::Push<unsigned int>(unsigned int count) {
    m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template <>
inline void VertexBufferLayout::Push<unsigned char>(unsigned int count) {
    m_Elements.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}