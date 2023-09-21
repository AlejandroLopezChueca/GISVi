#include <cstdint>
#include <glad/glad.h>

#include "OpenGLRendererAPI.h"

#include "graphics/renderer.h"

#include "graphics/rendererAPI.h"
#include "iostream"

#define GL_POINT_SMOOTH 0x0B10

GV::OpenGLRendererAPI::OpenGLRendererAPI(uint32_t indices[4])
  : m_Indices{indices[0], indices[1], indices[2], indices[3]} {}

void GV::OpenGLRendererAPI::setClearColor(const glm::vec4& color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void GV::OpenGLRendererAPI::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GV::OpenGLRendererAPI::drawElements(const GV::VertexArray& vertexArray, size_t count)
{
  unsigned int indices[6] = {0, 1, 3, 3, 1, 2};
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)&indices);
}

void GV::OpenGLRendererAPI::drawPoints(const GV::VertexArray& vertexArray, size_t count)
{
  glPointSize(GV::Renderer::s_PointSize);
  glDrawArrays(GL_POINTS, 0, count);
}
