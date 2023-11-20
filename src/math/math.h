#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace GV::Math
{
  bool decomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
  
  bool essentiallyEqual(float a, float b, float epsilon);

  float getMeanValueRaster(const std::vector<float>& data, const float noDataValue);

}
