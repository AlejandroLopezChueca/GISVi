#include <memory>

#include "OpenGL/OpenGLWindow.h"
#include "window.h"

std::unique_ptr<GV::Window> GV::Window::create(GV::API api, GV::PerspectiveCameraController* perspectiveCameraController)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLWindow>(perspectiveCameraController);
    case GV::API::NONE:
      return nullptr; 
  };
}

