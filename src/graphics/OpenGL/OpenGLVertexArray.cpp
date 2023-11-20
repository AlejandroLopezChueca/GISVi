#include "OpenGLVertexArray.h"
#include "graphics/buffer.h"
//#include <GL/gl.h>
#include <iostream>
#include <stdexcept>

namespace GV
{
  static GLenum ShaderDataTypeToOpenGLBaseType(GV::ShaderDataType type)
  {
    switch (type)
    {
      case ShaderDataType::Float: return GL_FLOAT;
      case ShaderDataType::Float2: return GL_FLOAT;
      case ShaderDataType::Float3: return GL_FLOAT;
      case ShaderDataType::Mat4: return GL_FLOAT;
      default:
	throw std::invalid_argument("Unknown ShaderDataType"); 
    }
    return 0;
  }
}

/////////////////////////////// OpenGLVertexBuffer ///////////////////////////////

GV::OpenGLVertexArray::OpenGLVertexArray()
{
  glCreateVertexArrays(1, &m_rendererID);
}

void GV::OpenGLVertexArray::bind() const
{
  glBindVertexArray(m_rendererID);
}

void GV::OpenGLVertexArray::unbind() const
{
  glBindVertexArray(0);
}

void GV::OpenGLVertexArray::addVertexBuffer(const VertexBuffer* vertexBuffer)
{
  if (vertexBuffer -> getLayout().getElements().size() == 0) throw std::invalid_argument("Vertex Buffer has no layout");
  // make sure that the vertex array is bound
  glBindVertexArray(m_rendererID);
  vertexBuffer -> bind();

  const auto& layout = vertexBuffer -> getLayout();
  for (const auto& element : layout)
  {
    switch (element.type)
    {
      case ShaderDataType::Float:
      case ShaderDataType::Float2:
      case ShaderDataType::Float3:
      {
	glEnableVertexAttribArray(m_vertexBufferIndex);
	glVertexAttribPointer(m_vertexBufferIndex,
	    element.getComponentCount(),
	    ShaderDataTypeToOpenGLBaseType(element.type),
	    element.normalized ? GL_TRUE : GL_FALSE,
	    layout.getStride(),
	    reinterpret_cast<const void*>(element.offset));

	if (element.instanced) glVertexAttribDivisor(m_vertexBufferIndex, 1);
	m_vertexBufferIndex += 1; 
	break;
      }
      case ShaderDataType::Mat4:
      {
	uint32_t count = element.getComponentCount();
	for (int i = 0; i < count; i++)
	{
	  glEnableVertexAttribArray(m_vertexBufferIndex);
	  glVertexAttribPointer(m_vertexBufferIndex, 
	      count,
	      ShaderDataTypeToOpenGLBaseType(element.type),
	      element.normalized ? GL_TRUE : GL_FALSE,
	      layout.getStride(),
	      reinterpret_cast<const void*>(element.offset + sizeof(float) * count * i));
	   if (element.instanced) glVertexAttribDivisor(m_vertexBufferIndex, 1);
	  m_vertexBufferIndex += 1;
	}
	break;
      }
      default:
	throw std::invalid_argument("Unknown ShaderDataType"); 
    }
  }
  vertexBuffers.push_back(vertexBuffer);
}

void GV::OpenGLVertexArray::setIndexBuffer(const GV::IndexBuffer *indexBuffer)
{
  glBindVertexArray(m_rendererID);
  indexBuffer -> bind();
  m_IndexBuffer = indexBuffer;
}

void GV::OpenGLVertexArray::updateMemberBufferData(const uint32_t idxBuffer, const uint32_t offset, const uint32_t size, const void* data)
{
  glNamedBufferSubData(vertexBuffers[idxBuffer] -> getVertexBufferID(), offset, size, data);
}
