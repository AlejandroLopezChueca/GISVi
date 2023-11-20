
#include "glm/ext/vector_float3.hpp"
#include <array>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>


namespace GV::ColorMaps
{
// Scientific color maps from https://www.fabiocrameri.ch/colourmaps/
  extern std::array<std::string, 4> nameColorMaps;
  extern std::array<const std::vector<glm::vec3>, 4> colorMaps;
}
