#pragma once

#include <cstdint>
#include <vector>
#include <glad/glad.h>

#include "graphics/vertexArray.h"
#include "graphics/buffer.h"
#include "OpenGLVertexArray.h"

namespace GV
{
//static GLenum ShaderDataTypeToOpenGLBaseType(FLB::ShaderDataType type); 

  class OpenGLVertexArray: public VertexArray
  {
    public:
      OpenGLVertexArray();

      virtual void bind() const override;
      virtual void unbind() const override;

      void addVertexBuffer(const VertexBuffer* vertexBuffer) override;
      void setIndexBuffer(const GV::IndexBuffer* indexBuffer) override;

      const std::vector<const VertexBuffer*>& getVertexBuffers() const override {return vertexBuffers;};

      void updateMemberBufferData(const uint32_t idxBuffer, const uint32_t offset, const uint32_t size, const void* data) override;

    private:
      uint32_t m_rendererID;
      uint32_t m_vertexBufferIndex = 0;
      std::vector<const VertexBuffer*> vertexBuffers;

      const IndexBuffer* m_IndexBuffer;

  };

}
