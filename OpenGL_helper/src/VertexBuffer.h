#pragma once
#include <cstdint>
#include <cstddef>

class VertexBuffer {
 private:
    unsigned int m_RendererID;

 public:
    VertexBuffer(const void* data, std::size_t size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
};
