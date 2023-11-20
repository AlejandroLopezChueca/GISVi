
#include "renderer.h"
#include "API.h"
#include "buffer.h"
//#include "font.h"
#include "camera.h"
#include "frameBuffer.h"
#include "glm/gtx/transform.hpp"
#include "rendererAPI.h"
#include "scene/components.h"
#include "scene/mesh.h"
#include "scene/scene.h"
#include "shader.h"
#include "texture.h"
#include "vertexArray.h"
#include "graphics/colorMaps.h"
#include "graphics/buffer.h"


#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/quaternion.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <iostream>
#include <glad/glad.h>
#include <vector>


//#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020

namespace GV 
{
  struct RendererData
  {
    std::unique_ptr<GV::Shader> lineShader;
    std::array<std::unique_ptr<GV::Shader>,4> cubeShaders;
    std::array<std::unique_ptr<GV::Shader>,2> terrainShaders;

    GV::Shader* terrainShaderInUse;
    GV::Shader* cubeShaderInUse;
    GV::Shader* monoInstancedCubeShaderInUse;
    GV::Shader* multiInstancedCubeShaderInUse;

    //std::vector<std::unique_ptr<GV::Texture1D>> colorMaps;
    std::array<std::unique_ptr<GV::Texture1D>, 4> colorMaps;
    //std::unique_ptr<GV::Texture1D> colorMapTerrain;
    //std::array<glm::vec4, 8> clippingPlaneData;
    
    // Uniform  BUffer of the view x projection matrix of the camera
    std::unique_ptr<GV::UniformBuffer> viewProjectionUniformBuffer;

    // data to perform clipping operations in the vertex shader
    std::array<glm::vec4, 8> clippingPlaneData;
    std::unique_ptr<GV::UniformBuffer> clippingPlaneUniformBuffer;


  };

  static RendererData s_RendererData;

}



//////////////////////////// Renderer ////////////////////////////

//Null initialization of renderer api
std::unique_ptr<GV::RendererAPI> GV::Renderer::s_RendererAPI = GV::RendererAPI::create();

entt::registry* GV::Renderer::s_Registry = nullptr;

bool GV::Renderer::s_UpdateRender = false;
float GV::Renderer::s_PointSize = 2.0f;


void GV::Renderer::init(GV::API api)
{ 
  GV::s_RendererData.lineShader = GV::Shader::create("assets/shaders/line.glsl", api);
  GV::s_RendererData.cubeShaders[0] = GV::Shader::create("assets/shaders/cube/cube.glsl", api);
  GV::s_RendererData.cubeShaders[1] = GV::Shader::create("assets/shaders/cube/cubeClipPlane.glsl", api);
  GV::s_RendererData.cubeShaders[2] = GV::Shader::create("assets/shaders/cube/cubeInstacingMonoEntity.glsl", api);
  GV::s_RendererData.cubeShaders[3] = GV::Shader::create("assets/shaders/cube/cubeInstacingMultiEntity.glsl", api);
  GV::s_RendererData.cubeShaderInUse = GV::s_RendererData.cubeShaders[0].get();
  GV::s_RendererData.monoInstancedCubeShaderInUse = GV::s_RendererData.cubeShaders[2].get();
  GV::s_RendererData.multiInstancedCubeShaderInUse = GV::s_RendererData.cubeShaders[3].get();

  GV::s_RendererData.terrainShaders[0] = GV::Shader::create("assets/shaders/terrain/terrain.glsl", api);
  GV::s_RendererData.terrainShaders[1] = GV::Shader::create("assets/shaders/terrain/terrainClipPlane.glsl", api);
  GV::s_RendererData.terrainShaderInUse = GV::s_RendererData.terrainShaders[0].get();


  // create the colormaps
  for (int i = 0; i < GV::s_RendererData.colorMaps.size(); i++) GV::s_RendererData.colorMaps[i] = GV::Texture1D::create(api, GV::ColorMaps::colorMaps[i]);

  {
    // size of the ViewProjection matrix (glm::mat4)
    int size = 4 * 4 * sizeof(float);
    GV::s_RendererData.viewProjectionUniformBuffer = GV::UniformBuffer::create(api, size, 0);
    GV::s_RendererData.viewProjectionUniformBuffer -> setData(0);
  }
 
  {
    // The max count of planes is 8 and for each plane there is a vec4
    int size = GV::s_RendererData.clippingPlaneData.size() * 4 * sizeof(float);
    GV::s_RendererData.clippingPlaneUniformBuffer = GV::UniformBuffer::create(api, size, 1);
    // Defaul init to 0
    GV::s_RendererData.clippingPlaneUniformBuffer -> setData(0);
  }

}

void GV::Renderer::beginScene(const GV::PerspectiveCamera& camera)
{
  const auto& viewProjectionMatrix = camera.getViewProjectionMatrix();
  GV::s_RendererData.viewProjectionUniformBuffer -> setData((void*)&viewProjectionMatrix);
}


void GV::Renderer::endScene()
{

}

void GV::Renderer::submit(const GV::VertexArray* vertexArray, size_t IndexCount, const GV::Shader& shader, const glm::mat4& viewProjectionMatrix)
{ 
  shader.bind();
  shader.setMat4("u_ViewProjection", viewProjectionMatrix);
  vertexArray -> bind();
  s_RendererAPI -> drawElements(IndexCount);
}

std::array<std::unique_ptr<GV::Texture1D>, 4>& GV::Renderer::getColorMaps()
{
  return GV::s_RendererData.colorMaps;
}

void GV::Renderer::setShadersToUse(int countClippingPlanes)
{
  if (countClippingPlanes == 0)
  {
    GV::s_RendererData.cubeShaderInUse = GV::s_RendererData.cubeShaders[0].get();
  GV::s_RendererData.monoInstancedCubeShaderInUse = GV::s_RendererData.cubeShaders[2].get();
  GV::s_RendererData.multiInstancedCubeShaderInUse = GV::s_RendererData.cubeShaders[3].get();
    GV::s_RendererData.terrainShaderInUse = GV::s_RendererData.terrainShaders[0].get();
  }
  else
  {
    GV::s_RendererData.cubeShaderInUse = GV::s_RendererData.cubeShaders[1].get();
    GV::s_RendererData.terrainShaderInUse = GV::s_RendererData.terrainShaders[1].get();
  }
}

void GV::Renderer::bindCubeShader()
{
  GV::s_RendererData.cubeShaderInUse -> bind();
}

void GV::Renderer::bindTerrainShader()
{
  GV::s_RendererData.terrainShaderInUse -> bind();
}

void GV::Renderer::sendDataClippingPlanes()
{
  GV::s_RendererData.clippingPlaneUniformBuffer->setData((void*)GV::s_RendererData.clippingPlaneData.data());
  //shader -> setFloatArray8("u_ClipPlane", GV::s_RendererData.clippingPlaneData);
}

void GV::Renderer::drawClippingPlane(const glm::vec3& planeNormal, const glm::vec3& pointOnPlane, const glm::vec3& rotation, int idx)
{
    glm::vec3 planeNormalUpdated = glm::toMat3(glm::quat(rotation)) * planeNormal;
  // paramter d of the plane ax +by +cz + d = 0
  float d = -glm::dot(planeNormalUpdated, pointOnPlane);
  GV::s_RendererData.clippingPlaneData[idx] = {planeNormalUpdated, d};
}

void GV::Renderer::drawCube(const GV::VertexArray* vertexArray, size_t indexCount, const glm::mat4 &transform, const glm::vec4& color)
{
  GV::s_RendererData.cubeShaderInUse -> bind();
  GV::s_RendererData.cubeShaderInUse->setMat4("u_Transform", transform);
  GV::s_RendererData.cubeShaderInUse->setFloat4("u_Color", color);
  vertexArray -> bind();

  s_RendererAPI -> drawElements(indexCount);
}

void GV::Renderer::drawInstancedCube(const GV::TransformComponent& transformComponent, const GV::CubeMeshComponent& cubeMeshComponent, entt::entity parentEntity)
{
  switch (cubeMeshComponent.typeInstancing) 
  {
    case GV::TypeInstancing::MonoEntity:
    {  
      GV::s_RendererData.monoInstancedCubeShaderInUse -> bind();
      GV::s_RendererData.monoInstancedCubeShaderInUse->setMat4("u_Transform", transformComponent.getTransform());
      GV::s_RendererData.monoInstancedCubeShaderInUse->setFloat4("u_Color", cubeMeshComponent.color);
      cubeMeshComponent.vertexArray -> bind();
      break;
    }
    case GV::TypeInstancing::MultiEntity:
    {
      GV::s_RendererData.multiInstancedCubeShaderInUse -> bind();
      GV::s_RendererData.multiInstancedCubeShaderInUse->setFloat4("u_Color", cubeMeshComponent.color);
      cubeMeshComponent.vertexArray -> bind();
      
      uint32_t childID = transformComponent.childID;
      // Update all the tranform buffer
      if (childID == 1)
      {
	// TODO Move to a compute shader
	std::vector<GV::Entity>& childs = s_Registry -> get<GV::ChildEntitiesComponent>(parentEntity).childEntities;
	std::vector<glm::mat4> childsTransform;
	childsTransform.reserve(childs.size());
      
	// Parent tranform
	glm::mat4 rotationMatrix = glm::toMat4(glm::quat(transformComponent.rotation));
	glm::mat4 identity = glm::mat4(1.0f);
	glm::mat4 scalePart = glm::scale(identity, transformComponent.scale);

	for (auto child : childs)
	{
	  auto& childTransformComponent = child.getComponent<GV::TransformComponent>();
	  glm::vec3 translationPart = transformComponent.translation + childTransformComponent.translation;

	  // save data to child
	  childTransformComponent.translation = translationPart;
	  childTransformComponent.rotation = transformComponent.rotation;
	  childTransformComponent.scale = transformComponent.scale;

	  childsTransform.push_back(glm::translate(identity, translationPart) * rotationMatrix * scalePart);
	}
	uint32_t size = sizeof(glm::mat4) * childs.size();
	cubeMeshComponent.vertexArray -> updateMemberBufferData(1, 0, size, reinterpret_cast<const void*>(childsTransform.data()));
      }
      // update the transofrm of the child with 
      // ID = childID - 2
      else if (childID > 1)
      {
	const GV::Entity& child = s_Registry -> get<GV::ChildEntitiesComponent>(parentEntity).childEntities[childID - 2];
	glm::mat4 childTransform = child.getComponent<GV::TransformComponent>().getTransform();
	uint32_t size = sizeof(glm::mat4);
	// the idx of the transform buffer is 1
	cubeMeshComponent.vertexArray -> updateMemberBufferData(1, (childID - 2) * size, size, reinterpret_cast<const void*>(&childTransform));
      }
      break; 
    }
  };
  s_RendererAPI -> drawInstancedElements(cubeMeshComponent.indexCount, cubeMeshComponent.instanceCount);
}
      
void GV::Renderer::drawTerrain(const GV::VertexArray* vertexArray, size_t indexCount, const glm::mat4& transform, const glm::mat4& viewProjectionMatrix, const glm::vec3& maxMinMeanHeight, int idxColorMap)
{
  // TODO this can be done once while the shader to use doesn't change
  //GV::s_RendererData.terrainShader -> bind();
  GV::s_RendererData.terrainShaderInUse -> setMat4("u_Transform", transform);
  GV::s_RendererData.terrainShaderInUse -> setFloat3("u_MaxMinMeanHeight", maxMinMeanHeight);
  GV::s_RendererData.colorMaps[idxColorMap] -> bind(0);

  vertexArray -> bind();
  s_RendererAPI -> drawElements(indexCount);
}

/*void GV::Renderer::drawString(const std::string& string)
{
  const auto& metrics = s_Font -> getFontGeometry() -> getMetrics();

  // font advance
  float x =0.0f;
  float fsScale = 1.0/(metrics.ascenderY - metrics.descenderY);
  float y = 0.0f;

  //In this way we cna treat every character as as char32_t


}*/
