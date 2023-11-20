#include <cstdint>
#include <glad/glad.h>

#include "OpenGLRendererAPI.h"

#include "graphics/renderer.h"

#include "graphics/rendererAPI.h"
#include "iostream"



void GV::OpenGLRendererAPI::setClearColor(const glm::vec4& color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void GV::OpenGLRendererAPI::clear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GV::OpenGLRendererAPI::drawElements(size_t indexCount) const
{
  //unsigned int idx[6] = {2, 3, 0, 0, 3 ,1};
  //unsigned int idx[6] = {0, 1, 3, 3, 1, 2};
  //glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)indices);
  glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void GV::OpenGLRendererAPI::drawInstancedElements(size_t indexCount, uint32_t instanceCount) const
{
  glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0,instanceCount);
}

void GV::OpenGLRendererAPI::drawPoints(const GV::VertexArray& vertexArray, size_t count)
{
  glPointSize(5);
  glDrawArrays(GL_POINTS, 0, count);
}
