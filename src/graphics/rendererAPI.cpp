

#include "rendererAPI.h"
#include "OpenGL/OpenGLRendererAPI.h"
#include <cstdint>

// default API
GV::API GV::RendererAPI::s_API = GV::API::NONE;

std::unique_ptr<GV::RendererAPI> GV::RendererAPI::create()
{
  switch (s_API) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLRendererAPI>();
    case GV::API::NONE:
      return nullptr;
    default:
      return nullptr;
  }

}


