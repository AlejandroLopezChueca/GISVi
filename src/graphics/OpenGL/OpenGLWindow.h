#pragma once

#include <cstdint>
#include <iostream>

#include <GLFW/glfw3.h>

#include "graphics/cameraController.h"
#include "graphics/window.h"
//#include "ui/editorLayer.h"

namespace GV
{
  class OpenGLWindow: public Window
  {

    public:
      OpenGLWindow();
      ~OpenGLWindow();

      void setVSync(bool enabled) override;
      void update() override;

      inline void* getWindow() const override {return m_renderWindow;};
      void setCameraController(GV::PerspectiveCameraController* cameraController) override {s_cameraController = cameraController;}
      void setGizmoOperation(int* gizmoOperation) override {s_GizmoOperation = gizmoOperation;}


    private:
      static void setScrollCallback(GLFWwindow* window,double xOffset, double yOffset);

      static void setKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

      static void debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

      GLFWwindow* m_renderWindow;
      static int* s_GizmoOperation;
      static bool s_GLFWInitialized;
      static GV::PerspectiveCameraController* s_cameraController;

    //friend class GV::EditorLayer;

  };

}
