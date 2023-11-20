#pragma once


#include "API.h"
#include "buffer.h"
#include "graphics/scene/components.h"
#include "graphics/scene/mesh.h"
#include "graphics/scene/scene.h"
#include "shader.h"
#include "frameBuffer.h"
#include "texture.h"
#include "vertexArray.h"
#include "rendererAPI.h"
#include "camera.h"
//#include "font.h"

#include <glm/glm.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include "glm/fwd.hpp"
#include <array>

namespace GV
{
  class Renderer
  {
    public:
      static void init(GV::API api);

      static void setRegistry(entt::registry* registry) {s_Registry = registry;};

      static void beginScene(const GV::PerspectiveCamera& camera);
      static void endScene();
      
      static void setClearColor(const glm::vec4& color) {s_RendererAPI -> setClearColor(color);}
      static void clear() {s_RendererAPI -> clear();}
      static void submit(const GV::VertexArray* vertexArray, size_t indexCount, const GV::Shader& shader, const glm::mat4& viewProjectionMatrix);
      static void setRendererAPI() {s_RendererAPI = GV::RendererAPI::create();}
      static std::array<std::unique_ptr<GV::Texture1D>, 4>& getColorMaps();
      static void setShadersToUse(int countClippingPlanes);
      //static void createQuad(GV::API api, Fl_Simple_Terminal* terminal);

      static void bindCubeShader();
      static void bindTerrainShader();

      static void sendDataClippingPlanes();

      static void drawClippingPlane(const glm::vec3& planeNormal, const glm::vec3& pointOnPlane,const glm::vec3& rotation, int idx);
      static void drawCube(const GV::VertexArray* vertexArray, size_t indexCount, const glm::mat4& transform, const glm::vec4& color);
      static void drawInstancedCube(const GV::TransformComponent& transformComponent, const GV::CubeMeshComponent& cubeMeshComponent, entt::entity parentEntity);
      static void drawTerrain(const GV::VertexArray* vertexArray, size_t indexCount, const glm::mat4& transform, const glm::mat4& viewProjectionMatrix, const glm::vec3& maxMinMeanHeight, int idxColorMap);

      static void drawString(const std::string& string);

      static bool s_UpdateRender;
      static float s_PointSize;
 
    private:
      static std::unique_ptr<GV::RendererAPI> s_RendererAPI;

      static entt::registry* s_Registry;

  };
}
