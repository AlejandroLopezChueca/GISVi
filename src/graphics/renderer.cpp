
#include "renderer.h"
#include "buffer.h"
//#include "font.h"
#include "frameBuffer.h"
#include "glm/fwd.hpp"
#include "rendererAPI.h"
#include "shader.h"
#include "texture.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <iostream>
#include <glad/glad.h>


//#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020

//////////////////////////// Renderer ////////////////////////////

//Null initialization of renderer api
std::unique_ptr<GV::RendererAPI> GV::Renderer::s_RendererAPI = GV::RendererAPI::create();

std::unique_ptr<GV::VertexBuffer> GV::Renderer::s_VertexBufferQuad = nullptr;

bool GV::Renderer::s_UpdateRender = false;
bool GV::Renderer::s_VelocityPointMode = true;
float GV::Renderer::s_PointSize = 2.0f;
bool GV::Renderer::s_VelocityTextureMode2D = false;

GV::Shader* GV::Renderer::s_shaderToUse = nullptr;
GV::Shader* GV::Renderer::s_shaderPointsVelocity = nullptr;
GV::Shader* GV::Renderer::s_shaderTextureVelocity2D = nullptr;

GV::FrameBuffer* GV::Renderer::s_TextureToUse = nullptr;
GV::FrameBuffer* GV::Renderer::s_TextureVelocity = nullptr;

//std::unique_ptr<GV::Font> GV::Renderer::s_Font = nullptr;
std::unique_ptr<GV::Texture2D> GV::Renderer::s_FontTexture = nullptr;


void GV::Renderer::beginScene()
{

}


void GV::Renderer::endScene()
{

}

void GV::Renderer::submit(const GV::VertexArray& vertexArray, const GV::Shader& shader, size_t count, const glm::mat4& viewProjectionMatrix)
{
  //if (!GV::Renderer::s_VelocityPointMode) s_TextureToUse->bind();
  
  s_shaderToUse -> bind();
  s_shaderToUse -> setMat4("u_ViewProjection", viewProjectionMatrix);
  vertexArray.bind();
  if (GV::Renderer::s_VelocityPointMode) s_RendererAPI -> drawPoints(vertexArray,count);
  else 
  {
    s_TextureToUse -> bindTexture();
    //s_shaderToUse -> setInt("u_ColorMap", 0);
    //s_shaderToUse -> setInt("u_Texture", 0);
    //s_TextureToUse -> unbind();
    //s_shaderToUse -> bind();
    //vertexArray.bind();
    //s_TextureToUse -> bindTexture();
    s_RendererAPI -> drawElements(vertexArray, count);
  }
}

/*void GV::Renderer::createQuad(GV::API api, Fl_Simple_Terminal* terminal)
{    
  float vertices[] =
    {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f

    };

  s_VertexBufferQuad = GV::VertexBuffer::create(api, terminal, vertices, sizeof(vertices));
  GV::BufferLayout layout = {
    {ShaderDataType::Float3, "a_QuadPoints"},
    {ShaderDataType::Float2, "a_QuadTextCoord"}
  };
  s_VertexBufferQuad -> setLayout(layout);
}*/

/*void GV::Renderer::drawString(const std::string& string)
{
  const auto& metrics = s_Font -> getFontGeometry() -> getMetrics();

  // font advance
  float x =0.0f;
  float fsScale = 1.0/(metrics.ascenderY - metrics.descenderY);
  float y = 0.0f;

  //In this way we cna treat every character as as char32_t


}*/
