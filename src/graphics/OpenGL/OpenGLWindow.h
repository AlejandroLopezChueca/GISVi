#pragma once

#include <cstdint>
#include <iostream>

#include <GLFW/glfw3.h>

#include "graphics/window.h"

namespace GV
{
  class OpenGLWindow: public Window
  {

    public:
      OpenGLWindow(GV::PerspectiveCameraController* cameraController);
      ~OpenGLWindow();

      void setVSync(bool enabled) override;
      void update() override;

      inline void* getWindow() const override {return m_renderWindow;};

      //bool isKeyPressed(int keyCode) override;

      //bool render;
      //bool a = render;

    private:
      static void setScrollCallback(GLFWwindow* window,double xOffset, double yOffset);

      static void setKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

      GLFWwindow* m_renderWindow;
      static bool s_GLFWInitialized;
      static GV::PerspectiveCameraController* s_cameraController;
      /*struct WindowData
      {
	bool& isRendering = render;
      };

      WindowData windowData;*/

  };

}
