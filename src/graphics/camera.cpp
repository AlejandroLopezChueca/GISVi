#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "camera.h"
#include  <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>


GV::PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
  : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip),  m_ProjectionMatrix(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
{
  //m_Position = calculatePosition();
  updateViewMatrix();
}

void GV::PerspectiveCamera::updateProjectionMatrix()
{
  m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
  m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
}

void GV::PerspectiveCamera::updateViewMatrix()
{
  // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
  m_Position = calculatePosition();

  glm::quat orientation = getOrientation();
  m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
  m_ViewMatrix = glm::inverse(m_ViewMatrix);
}

glm::vec3 GV::PerspectiveCamera::getUpDirection() const
{
  return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 GV::PerspectiveCamera::getRightDirection() const
{
  return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 GV::PerspectiveCamera::getForwardDirection() const
{
  return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::quat GV::PerspectiveCamera::getOrientation() const
{
  return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}

glm::vec3 GV::PerspectiveCamera::calculatePosition() const
{
  return m_FocalPoint - getForwardDirection() * m_Distance;
}

