
#include "OpenGLBuffer.h"
#include "graphics/buffer.h"
#include <cstddef>
#include <iostream>
#include <cstdint>

namespace GV
{
  static GLenum BufferTypeDrawToOpenGL(GV::BufferTypeDraw bufferTypeDraw)
  {
    switch (bufferTypeDraw) 
    {
      case GV::BufferTypeDraw::Static:
	return GL_STATIC_DRAW;

      case GV::BufferTypeDraw::Dynamic:
	return GL_DYNAMIC_DRAW;

      case GV::BufferTypeDraw::Stream:
	return GL_DYNAMIC_DRAW;    
    };

  }
}


///////////////////////// VertexBuffer ////////////////////////////
//
GV::OpenGLVertexBuffer::OpenGLVertexBuffer(void* vertices, int size, GV::BufferTypeDraw bufferTypeDraw)
{
  glCreateBuffers(1, &m_rendererID);
  glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GV::BufferTypeDrawToOpenGL(bufferTypeDraw));
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

void GV::OpenGLVertexBuffer::updateDynamicData()
{

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
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void GV::OpenGLIndexBuffer::unbind() const
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

///////////////////////// UniformBuffer ///////////////////////////////

GV::OpenGLUniformBuffer::OpenGLUniformBuffer(int size, uint32_t bindingPoint) 
: m_Size(size)
{
  glCreateBuffers(1, &m_RendererID);
  glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
  glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID);
}

GV::OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
  glDeleteBuffers(1, &m_RendererID);
}

void GV::OpenGLUniformBuffer::bind() const
{
  glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
}

void GV::OpenGLUniformBuffer::unbind() const
{
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GV::OpenGLUniformBuffer::setData(const void* data) const
{
  glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, m_Size, data);
}
