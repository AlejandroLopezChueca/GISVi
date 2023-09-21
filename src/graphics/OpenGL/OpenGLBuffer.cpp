#include <cstdint>

#include "OpenGLBuffer.h"
#include <iostream>

///////////////////////// VertexBuffer ///////////////////////////////
///
GV::OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, int size)
{
  glCreateBuffers(1, &m_rendererID);
  glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

}

GV::OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
  glDeleteBuffers(1, &m_rendererID);
}

void GV::OpenGLVertexBuffer::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void GV::OpenGLVertexBuffer::unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

///////////////////////// IndexBuffer ///////////////////////////////

GV::OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
{
  glCreateBuffers(1, &m_rendererID);
  // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
 // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
  glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
  glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

}

GV::OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
  glDeleteBuffers(1, &m_rendererID);
}

void GV::OpenGLIndexBuffer::bind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void GV::OpenGLIndexBuffer::unbind() const
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
