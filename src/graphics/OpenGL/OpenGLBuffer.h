#pragma once

#include "graphics/buffer.h"

#include <cstdint>
#include <glad/glad.h>

namespace GV
{
  class OpenGLVertexBuffer: public VertexBuffer
  {
    public:
      OpenGLVertexBuffer(void* vertices, int size, GV::BufferTypeDraw bufferTypeDraw);
      ~OpenGLVertexBuffer();

      void bind() const override;
      void unbind() const override;
      void updateDynamicData() override;
      const BufferLayout& getLayout() const override {return m_layout;}
      void setLayout(const BufferLayout& layout) override {m_layout = layout;}
      unsigned int getVertexBufferID() const override {return m_rendererID;}
    
    private:
      unsigned int m_rendererID;
      BufferLayout m_layout;
  };
  
  class OpenGLIndexBuffer: public IndexBuffer
  {
    public:
      OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
      ~OpenGLIndexBuffer();

      void bind() const override;
      void unbind() const override;
    
    private:
      uint32_t m_rendererID;
  };

  class OpenGLUniformBuffer: public UniformBuffer
  {
    public:
      OpenGLUniformBuffer(int size, uint32_t bindingPoint);
      ~OpenGLUniformBuffer();

      void bind() const override;
      void unbind() const override;
      void setData(const void* data) const override;

    private:
      uint32_t m_RendererID;
      int m_Size;
  };
}
