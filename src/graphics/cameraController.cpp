#include "cameraController.h"
#include "GLFW/glfw3.h"
#include "glm/fwd.hpp"
#include "scene/components.h"

#include <algorithm>
#include <iostream>
#include <utility>

GV::PerspectiveCameraController::PerspectiveCameraController(float fov,float aspectRatio, float nearClip, float farClip)
  :m_Camera(fov, aspectRatio, nearClip, farClip)
{}

void GV::PerspectiveCameraController::GLFWUpdate(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
  {
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    const glm::vec2 mouse{xPos, yPos};
    glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
    m_InitialMousePosition = mouse; 

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) mousePan(delta);
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) mouseRotate(delta);
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) mouseZoom(delta.y);
    m_Camera.updateViewMatrix();
  }
}

void GV::PerspectiveCameraController::resize(float width, float height)
{
  m_Camera.m_ViewportWidth = width;
  m_Camera.m_ViewportHeight = height;
  m_Camera.updateProjectionMatrix();
}

void GV::PerspectiveCameraController::onMouseScrolled(float xOffset, float yOffset)
{
  mouseZoom(yOffset * 0.1f);
  m_Camera.updateViewMatrix();
}

void GV::PerspectiveCameraController::mousePan(const glm::vec2& delta)
{
  auto [xSpeed, ySpeed] = getPanSpeed();
  m_Camera.m_FocalPoint += -m_Camera.getRightDirection() * delta.x * xSpeed * m_Camera.m_Distance * m_CameraTranslationSpeedFactor;
  m_Camera.m_FocalPoint += m_Camera.getUpDirection() * delta.y * ySpeed * m_Camera.m_Distance * m_CameraTranslationSpeedFactor;
}

void GV::PerspectiveCameraController::mouseRotate(const glm::vec2 delta)
{
  float yawSign = m_Camera.getUpDirection().y < 0 ? -1.0f : 1.0f;
  m_Camera.m_Yaw += yawSign * delta.x * rotationSpeed();
  m_Camera.m_Pitch += delta.y * rotationSpeed();
}

void GV::PerspectiveCameraController::mouseZoom(float delta)
{
  m_Camera.m_Distance -= delta * zoomSpeed();
  if (m_Camera.m_Distance < 1.0f)
  {
    m_Camera.m_FocalPoint += m_Camera.getForwardDirection();
    m_Camera.m_Distance = 1.0f;
  }
}

void GV::PerspectiveCameraController::zoomToEntity(GV::Entity &entity)
{
  auto& transformComponent = entity.getComponent<GV::TransformComponent>();
  m_Camera.m_FocalPoint = transformComponent.translation;
  m_Camera.updateViewMatrix();
}

void GV::PerspectiveCameraController::setViewportSize(float width, float height)
{
  m_Camera.m_ViewportWidth = width;
  m_Camera.m_ViewportHeight = height;
  m_Camera.updateProjectionMatrix();
}

std::pair<float, float> GV::PerspectiveCameraController::getPanSpeed() const
{	
  float x = std::min(m_Camera.m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
  float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

  float y = std::min(m_Camera.m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
  float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
  return { xFactor, yFactor };
}

float GV::PerspectiveCameraController::zoomSpeed() const
{
  float distance = m_Camera.m_Distance * 0.2f;
  distance = std::max(distance, 0.0f);
  float speed = distance * distance;
  speed = std::min(speed, 100.0f); // max speed = 100
  return speed;
}

float GV::PerspectiveCameraController::rotationSpeed() const
{
  return m_CameraRotationSpeed;
}

