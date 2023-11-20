
#pragma once

#include "graphics/cameraController.h"

namespace GV
{
  // Forward declaration
  class EditorLayer;

  class SettingsPanel
  {
    public:
      SettingsPanel() = default;
      SettingsPanel(GV::EditorLayer* editorLayer);

      void onImGuiRender(bool* open);
      
      void setCameraController(GV::PerspectiveCameraController* cameraController) {m_CameraController = cameraController;}

      private:
	void showCameraSettings();

	GV::EditorLayer* m_EditorLayer;
	GV::PerspectiveCameraController* m_CameraController;
	
  };

}
