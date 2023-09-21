#pragma once

#include "glm/ext/vector_float3.hpp"
#include "graphics/texture.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

namespace GV
{
  class OpenGLTexture1D: public Texture1D
  {
    public:
      OpenGLTexture1D(const std::vector<glm::vec3>& colors);
      ~OpenGLTexture1D();

      uint32_t getWidth() const override {return m_Width;}
      uint32_t getHeight() const override {return m_Height;}
      uint32_t getID() const override {return m_RendererID;}

      void bind(uint32_t slot) const override;
      void setColors(const std::vector<glm::vec3>& colors) override;
      void setData(void* data, uint32_t size) override;

    private:
      uint32_t m_Width;
      uint32_t m_Height;
      uint32_t m_RendererID;

  };

  class OpenGLTexture2D: public Texture2D
  {
    public:
      OpenGLTexture2D(uint32_t width, uint32_t height, uint32_t channels);
      ~OpenGLTexture2D();

      uint32_t getWidth() const override {return m_Width;}
      uint32_t getHeight() const override {return m_Height;}
      uint32_t getID() const override {return m_RendererID;}

      void bind(uint32_t slot) const override;

      void setData(void* data, uint32_t size) override;

    private:
      uint32_t m_Width;
      uint32_t m_Height;
      uint32_t m_RendererID;
      GLenum m_InternalFormat;
      GLenum m_DataFormat;
  };
}
