

#include "rendererAPI.h"
#include "OpenGL/OpenGLRendererAPI.h"
#include <cstdint>

// default API
GV::API GV::RendererAPI::s_API = GV::API::NONE;

std::unique_ptr<GV::RendererAPI> GV::RendererAPI::create(uint32_t indices[4])
{
  switch (s_API) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLRendererAPI>(indices);
    case GV::API::NONE:
      return nullptr; 
  }

}


