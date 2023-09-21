#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "API.h"

namespace GV 
{
  class Shader
  {
    public:
      virtual ~Shader() = default;

      virtual void bind() const = 0;
      virtual void unbind() const = 0;

      virtual void setInt(const std::string& name, int value) = 0;
      virtual void setMat4(const std::string& name, const glm::mat4& matrix) const = 0;

      static std::unique_ptr<Shader> create(const std::string& filePath, GV::API api);
      static std::unique_ptr<Shader> create(const std::string& vertexSrc, const std::string& fragmentSrc, GV::API api);
  };

}
