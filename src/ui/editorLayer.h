#pragma once


#include "graphics/API.h"
#include "graphics/frameBuffer.h"
#include "graphics/layer.h"
#include "graphics/scene/mesh.h"
#include "graphics/shader.h"
#include "graphics/cameraController.h"
#include "panels/addLayersPanel.h"
#include "panels/logPanel.h"
#include "panels/hierarchyPanel.h"
#include "panels/settingsPanel.h"
#include "panels/contentBrowserPanel.h"
#include "graphics/scene/scene.h"
#include "graphics/texture.h"

#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace GV 
{ 


  class EditorLayer: public Layer
  {
    public:
      EditorLayer(GV::API api);
      ~EditorLayer() = default;

      void onUpdate() override;
      
      void onAttach() override;
      void onDetach() override;
      void onImGuiRender() override;

      void newProject();

      GV::PerspectiveCameraController* getCameraController() {return m_CameraController.get();}

      void setGLFWwindow(GLFWwindow* window) {m_GLFWwindow = window;}

      int* getGizmoOperation() {return &m_GizmoOperation;}

      // Panels
      void UIToolBar();
      void UILayersPanel();
      void UILog();

      void UITerrain(bool* open);
      void UIHDF5(bool* open);
      void UICubesFromPoints(bool* open);

    private:

      void addClippingPlane();

      std::unique_ptr<GV::TerrainMesh> m_TerrainMesh;
      std::unique_ptr<GV::Shader> m_TerrainShader;

      std::unique_ptr<GV::FrameBuffer> m_Framebuffer;
      
      std::unique_ptr<GV::PerspectiveCameraController> m_CameraController;

      std::unique_ptr<GV::Scene> m_Scene;
      
      glm::vec2 m_ViewportSize = {0.0f, 0.0f};
      glm::vec2 m_ViewportBounds[2];
      bool m_ViewportFocused = false;
      bool m_ViewportHovered = false;

      GV::API m_Api;

      GLFWwindow* m_GLFWwindow;

      // Panels
      HierarchyPanel m_HierarchyPanel;
      ImGuiLogPanel m_LogPanel;
      SettingsPanel m_SettingsPanel{this};
      ContentBrowserPanel m_ContentBrowserPanel;
      HECRASHDF5Panel m_HECRASHDF5Panel;
      TerrainPanel m_TerrainPanel;

      CubesFromPointsPanel m_CubesFromPointsPanel;
      // Resources
      std::unique_ptr<GV::Texture2D> m_IconGizmo;


      bool m_ShowUISettings = false;
      bool m_ShowUIHDF5 = false;
      bool m_ShowUICubesFromPoints = false;
      bool m_showUITerrain = false;
      
      bool m_UseGizmos = false;
      int m_GizmoOperation = -1; // Translate = 0, Rotate = 1, Scale = 2, Bounds = 3


  };

}
