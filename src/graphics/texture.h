#pragma once

#include "glm/ext/vector_float3.hpp"
#include "graphics/API.h"
#include <cstdint>
#include <memory>
#include <glm/glm.hpp>
#include <sys/types.h>
#include <vector>
#include <filesystem>


namespace GV
{
  enum class ImageFormat
  {
    None = 0,
    R8,
    RGB8,
    RGBA8,
    RGBA32F
  };


  class Texture
  {
    public:
      virtual ~Texture() = default;

      virtual uint32_t getWidth() const = 0;
      virtual uint32_t getHeight() const = 0;
      virtual uint32_t getID() const = 0;

      virtual void bind(uint32_t slot) const = 0;

      virtual void setData(void* data, uint32_t size) = 0;
  };

  class Texture1D: public Texture
  {
    public:
      virtual ~Texture1D() = default;
      virtual void setColors(const std::vector<glm::vec3>& colors) = 0;
      static std::unique_ptr<Texture1D> create(GV::API api, const std::vector<glm::vec3>& colors); 
  };

  class Texture2D: public Texture
  {
    public:
      static std::unique_ptr<Texture2D> create(GV::API api, uint32_t width, uint32_t height, uint32_t channels);

      static std::unique_ptr<Texture2D> create(GV::API api, const std::filesystem::path& path);
  };

}
