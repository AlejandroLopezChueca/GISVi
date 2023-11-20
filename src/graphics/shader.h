#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "API.h"
#include "glm/fwd.hpp"

namespace GV 
{
  class Shader
  {
    public:
      virtual ~Shader() = default;

      virtual void bind() const = 0;
      virtual void unbind() const = 0;

      virtual void setInt(const std::string& name, int value) = 0;
      virtual void setFloat(const std::string& name, float value) = 0;
      virtual void setFloat2(const std::string& name, const glm::vec2& vector) const = 0;
      virtual void setFloat3(const std::string& name, const glm::vec3& vector) const = 0;
      virtual void setFloat4(const std::string& name, const glm::vec4& vector) const = 0;
      virtual void setMat4(const std::string& name, const glm::mat4& matrix) const = 0;
      virtual void setFloatArray8(const std::string& name, const std::array<glm::vec4, 8>& arrayData) const = 0;

      static std::unique_ptr<Shader> create(const std::string& filePath, GV::API api);
      static std::unique_ptr<Shader> create(const std::string& vertexSrc, const std::string& fragmentSrc, GV::API api);
  };

}
