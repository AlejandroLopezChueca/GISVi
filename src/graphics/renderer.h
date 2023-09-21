#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "buffer.h"
#include <glm/glm.hpp>
//#include "glm/fwd.hpp"
#include "graphics/shader.h"
#include "graphics/frameBuffer.h"
#include "texture.h"
#include "vertexArray.h"
#include "rendererAPI.h"
#include "camera.h"
//#include "font.h"

namespace GV
{
  class Renderer
  {
    public:
      static void beginScene();
      static void endScene();
      
      static void setClearColor(const glm::vec4& color) {s_RendererAPI -> setClearColor(color);}
      static void clear() {s_RendererAPI -> clear();}
      static void submit(const GV::VertexArray& vertexArray, const GV::Shader& shader, size_t count, const glm::mat4& viewProjectionMatrix);
      static void setRendererAPI(uint32_t indices[4]) {s_RendererAPI = GV::RendererAPI::create(indices);}
      //static void createQuad(GV::API api, Fl_Simple_Terminal* terminal);

      static void drawString(const std::string& string);

      static bool s_UpdateRender;
      static bool s_VelocityPointMode;
      static float s_PointSize;
      static bool s_VelocityTextureMode2D;

      static std::unique_ptr<GV::VertexBuffer> s_VertexBufferQuad; 

      // Pointers to data of dinamic use in rendering
      static GV::FrameBuffer* s_TextureToUse;
      static GV::FrameBuffer* s_TextureVelocity;

      static GV::Shader* s_shaderToUse;
      static GV::Shader* s_shaderPointsVelocity;
      static GV::Shader* s_shaderTextureVelocity2D;

      // font used to render strings of text
      //static std::unique_ptr<GV::Font> s_Font; 
      static std::unique_ptr<Texture2D> s_FontTexture;
      static std::unique_ptr<GV::Shader> s_TextShader;
   
    private:
      static std::unique_ptr<GV::RendererAPI> s_RendererAPI;

      // max quads to render in onne draw call
      static const uint32_t maxQuads = 20;
  };
}
