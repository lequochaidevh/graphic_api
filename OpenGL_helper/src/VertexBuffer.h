#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
class VertexBuffer {
 private:
    unsigned int m_RendererID;

 public:
    VertexBuffer(const void* data, std::size_t size,
                 std::string allocateType = "static");
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
};
