#pragma once

#include "camera.h"
#include "graphics/camera.h"
#include "graphics/scene/scene.h"
#include "scene/entity.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <memory>
#include <utility>

namespace GV
{
  class PerspectiveCameraController
  {
    public:
      PerspectiveCameraController(float fov, float aspectRatio, float nearClip, float farClip);

      void GLFWUpdate(GLFWwindow* window);
      void resize(float widht, float height);
      void onMouseScrolled(float xOffset, float yOffset);
      void mousePan(const glm::vec2& delta);
      void mouseRotate(const glm::vec2 delta);
      void mouseZoom(float delta);

      void zoomToEntity(GV::Entity& entity);

      void setViewportSize(float width, float height);

      float& getCameraTranslationSpeed() {return m_CameraTranslationSpeedFactor;}
      float& getCameraRotationSpeed() {return m_CameraRotationSpeed;}
      float& getCameraNearClip() {return m_Camera.m_NearClip;}
      float& getCameraFarClip() {return m_Camera.m_FarClip;}


      const GV::PerspectiveCamera& getCamera() const {return m_Camera;}

      void updateCameraProjectionMatrix() {m_Camera.updateProjectionMatrix();}
      void setZoomLevel(float zoomLevel) {m_ZoomLevel = zoomLevel;}


    private:

      std::pair<float, float> getPanSpeed() const;
      float zoomSpeed() const;
      float rotationSpeed() const;

      float m_ZoomLevel = 1.0f;
      GV::PerspectiveCamera m_Camera;

      glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

      //glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
      float m_CameraTranslationSpeedFactor = 1.0f;
      float m_CameraRotationSpeed = 0.8f;
  };

}
