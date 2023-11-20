#pragma once

#include "graphics/rendererAPI.h"
#include <cstddef>
#include <cstdint>

namespace GV
{

  class OpenGLRendererAPI: public RendererAPI
  {
    public:
      //OpenGLRendererAPI();

      void setClearColor(const glm::vec4& color) override;
      void clear() override;

      void drawElements(size_t indexCount) const override;
      void drawInstancedElements(size_t indexCount, uint32_t instaceCount) const override;
      void drawPoints(const GV::VertexArray& vertexArray, size_t count) override;

    private:
      uint32_t m_Indices[4];
  };

  class OpenGLRendererAPI2D
  {

  };

}
