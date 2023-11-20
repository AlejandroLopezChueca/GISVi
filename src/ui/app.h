#pragma once

#include <functional>
#include <memory>

//#include "io/reader.h"
#include "entryLayer.h"
#include "editorLayer.h"
#include "graphics/window.h"
#include "ImGuiLayer.h"
#include "graphics/layerStack.h"



namespace GV
{
  class App
  {
    public:
      App();
      ~App();

      void startApp();
      void run();
      
      void pushLayer(Layer* layer);
      void pushOverlay(Layer* layer);

      GV::Window& getRenderWindow() {return *m_RenderWindow;}

      GV::ImGuiLayer* getImGuiLayer() {return m_ImGuiLayer;}

      bool getInitApp() {return m_InitApp;}

      static void close() {s_Running = false;}

      static App& getApp() {return *s_Instance;} 

      static bool s_appCreated;


      //std::string pathFiles;
    private:
      std::unique_ptr<GV::Window> m_RenderWindow;
      GV::ImGuiLayer* m_ImGuiLayer;
      GV::EditorLayer* m_EditorLayer;

      LayerStack m_LayerStack;

      bool m_Minimized = false;
      bool m_InitApp = false;
      GV::API m_Api;

      GV::EntryLayer* m_Launcher;

      static bool s_Running;
      static App* s_Instance;
      //std::unique_ptr<FLB::OrthographicCameraController> m_OrthographicCameraController;
  };

}
