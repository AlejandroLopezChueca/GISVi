#include <memory>

#include "vertexArray.h"
#include "graphics/buffer.h"
#include "OpenGL/OpenGLVertexArray.h"

std::unique_ptr<GV::VertexArray> GV::VertexArray::create(GV::API api)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLVertexArray>();
    case GV::API::NONE:
      return nullptr;
  
  };
}

