#include "frameBuffer.h"
#include "OpenGL/OpenGLFrameBuffer.h"
#include "graphics/API.h"

#include <iostream>

std::unique_ptr<GV::FrameBuffer> GV::FrameBuffer::create(GV::API api, const FrameBufferSpecifications& specs)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLFrameBuffer>(specs);

    case GV::API::NONE:
      return nullptr;
  }

}
