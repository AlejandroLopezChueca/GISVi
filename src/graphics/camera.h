#pragma once

#include "glm/fwd.hpp"
#include <glm/glm.hpp>


namespace GV
{
  class PerspectiveCamera
  {
    public:
      PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);

      const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
      const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
      // glm order, column major
      const glm::mat4 getViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix;}


      glm::vec3 getUpDirection() const;
      glm::vec3 getRightDirection() const;
      glm::vec3 getForwardDirection() const;
      const glm::vec3& getPosition() const {return  m_Position;}
      glm::quat getOrientation() const;

      void updateViewMatrix();
      void updateProjectionMatrix();

      float m_ViewportWidth = 1280;
      float m_ViewportHeight = 720;
      glm::vec3 m_FocalPoint = {0.0f, 0.0f, 0.0f};
      float m_Distance = 10.0f;

      float m_Pitch = 0.0f;
      float m_Yaw = 0.0f;

    private:
      glm::vec3 calculatePosition() const;

      float m_FOV = 45.0f;
      float m_AspectRatio = 1.778f;
      float m_NearClip = 0.1f;
      float m_FarClip = 1000.0f;

      glm::mat4 m_ProjectionMatrix;
      glm::mat4 m_ViewMatrix;

      glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};

      friend class PerspectiveCameraController;
      friend class SettingsPanel;



  };
}
