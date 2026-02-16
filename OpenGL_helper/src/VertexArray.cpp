#include "VertexArray.h"
#include "Renderer.h"

#include "VertexBufferLayout.h"

#include <iostream>
VertexArray::VertexArray() { GLCall(glGenVertexArrays(1, &m_RenderID)); }

VertexArray::~VertexArray() { GLCall(glDeleteVertexArrays(1, &m_RenderID)); }

void VertexArray::AddBuffer(const VertexBuffer&       vb,
                            const VertexBufferLayout& layout) {
    this->Bind();
    vb.Bind();
    const auto& elements = layout.GetElement();

    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        // std::cout << "[DEBUG] Attribute: " << i << " | Count: " <<
        // element.count
        //           << " | Type: " << element.type << " | Normalized: "
        //           << (element.normalized ? "TRUE" : "FALSE")
        //           << " | Stride: " << layout.GetStride()
        //           << " | Offset: " << offset << std::endl;
        // 1. Set the attribute pointer using the CURRENT offset
        GLCall(glVertexAttribPointer(
            i,                        // Attribute index
            element.count,            // Number of components (e.g. 3 for RGB)
            element.type,             // GL_FLOAT, etc.
            element.normalized,       // GL_TRUE/GL_FALSE
            layout.GetStride(),       // Total size of one vertex
            (void*)(uintptr_t)offset  // The calculated offset
            ));

        GLCall(glEnableVertexAttribArray(i));  // Don't forget to enable!
        // 2. Update the offset for the NEXT element
        offset +=
            element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

void VertexArray::Bind() const {
    GLCall(glBindVertexArray(m_RenderID));
    //
}

void VertexArray::Unbind() const { GLCall(glBindVertexArray(0)); }
