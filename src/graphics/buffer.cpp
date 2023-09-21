#include "buffer.h"
#include "OpenGL/OpenGLBuffer.h"
#include <cstdint>


static uint32_t GV::ShaderDataTypeSyze(ShaderDataType type)
  {
    switch (type) 
    { 
      case ShaderDataType::Float:  return 4;
      case ShaderDataType::Float2: return 4 * 2;
      case ShaderDataType::Float3: return 4 * 3; 
    }
    return 0;
}

////////////////////////// BufferElement ////////////////////////// 

GV::BufferElement::BufferElement(GV::ShaderDataType type_ ,const std::string name_, bool normalized_)
  : name(name_), type(type_), size(GV::ShaderDataTypeSyze(type_)), offset(0), normalized(normalized_) {}

uint32_t GV::BufferElement::getComponentCount() const
{
  switch (type)
  {
    case ShaderDataType::Float:   return 1;
    case ShaderDataType::Float2:  return 2;
    case ShaderDataType::Float3:  return 3;
  }
  return 0;
}


////////////////////////// BufferLayout ////////////////////////// 

GV::BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements_)
  : elements(elements_) {calculateOffsetsAndStride();}

void GV::BufferLayout::calculateOffsetsAndStride()
{
  size_t offset = 0;
  stride = 0;
  for (auto& element : elements)
  {
	element.offset = offset;
	offset += element.size;
	stride += element.size;
  }
}

////////////////////////// VertexBuffer ////////////////////////// 

std::unique_ptr<GV::VertexBuffer> GV::VertexBuffer::create(GV::API api, float* vertices, int size)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLVertexBuffer>(vertices, size);
    case GV::API::NONE:
      return nullptr;
  
  };
}

////////////////////////// IndexBuffer ////////////////////////// 

std::unique_ptr<GV::IndexBuffer> GV::IndexBuffer::create(GV::API api, uint32_t* indices, uint32_t count)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLIndexBuffer>(indices, count);
    case GV::API::NONE:
      return nullptr;
  
  };
}
