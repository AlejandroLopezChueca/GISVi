#include <memory>

#include "OpenGL/OpenGLWindow.h"
#include "window.h"

std::unique_ptr<GV::Window> GV::Window::create(GV::API api)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLWindow>();
    case GV::API::NONE:
      return nullptr;
    default:
      return nullptr;
  };
}

