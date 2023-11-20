#pragma once

#include "graphics/frameBuffer.h"
#include <cstdint>

namespace GV
{
  class OpenGLFrameBuffer: public FrameBuffer
  {
    public:
      OpenGLFrameBuffer(const GV::FrameBufferSpecifications& specs);
      ~OpenGLFrameBuffer();

      void create();

      void bind() override;
      void bindTexture() override;
      void unbind() override;
      
      void resize(uint32_t width, uint32_t height) override;

      uint32_t getColorAttachmentID() const override {return m_ColorAttachment;}
      const FrameBufferSpecifications& getSpecifications() const override {return m_Specifications;}


    private:
      uint32_t m_RendererID;
      uint32_t m_ColorAttachment;
      uint32_t m_DepthAttachment;
      FrameBufferSpecifications m_Specifications;

  };
}
