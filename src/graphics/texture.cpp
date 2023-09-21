#include "texture.h"
#include "OpenGL/OpenGLTexture.h"
#include "colorMaps.h"
#include <cstdint>
#include <vector>

std::unique_ptr<GV::Texture1D> GV::Texture1D::create(GV::API api)
{
  std::vector<glm::vec3> defaultColor = GV::ColorMaps::colorMaps["imola"];
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLTexture1D>(defaultColor);
    case GV::API::NONE:
      return nullptr; 
  };
}

std::unique_ptr<GV::Texture2D> GV::Texture2D::create(GV::API api, uint32_t width, uint32_t height, uint32_t channels)
{
  switch (api) 
  {
    case GV::API::OPENGL:
      return std::make_unique<GV::OpenGLTexture2D>(width, height, channels);
    case GV::API::NONE:
      return nullptr; 
  };
}
