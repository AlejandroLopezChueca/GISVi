#pragma once

#include <GLFW/glfw3.h>
#include <string>

#include "graphics/API.h"

namespace GV
{
  class EntryLayer
  {
    public:
      EntryLayer(GV::API& api, bool& initApp);
      ~EntryLayer();

      void start();

      void imGuiAttach();
      void imGuiBegin();
      void imGuiEnd();

      void showUI(); 

      float getWidth() {return m_Width;}
      float getHeight() {return m_Height;}

      void getGPUInfo();

    private:
      GLFWwindow* m_Window;
      float m_Width = 550;
      float m_Height = 280;
      std::string m_Title = "GISVi";
      
      bool& m_InitApp;

      static bool s_StopRendering;

      std::string m_OpenGLVersion;
      std::string m_Vendor;
      std::string m_Renderer;

      GV::API& m_Api;
  };

}
