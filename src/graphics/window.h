#pragma once

#include <cstdint>
#include <memory>

#include "renderer.h"
#include "cameraController.h"

namespace GV 
{
  class Window
  {
    public:

      virtual ~Window() = default;

      virtual void setVSync(bool enabled) = 0;
      virtual void update() = 0;

      virtual void* getWindow() const = 0;
      uint32_t getWidth() const {return m_Width;}
      uint32_t getHeight() const {return m_Height;}

      virtual void setCameraController(GV::PerspectiveCameraController* cameraController) = 0;
      virtual void setGizmoOperation(int* gizmoOperation) = 0;

      //virtual bool isKeyPressed(int keyCode) = 0;

      static std::unique_ptr<Window> create(GV::API api);

    protected:
      const std::string title = "GISVi 0.1";
      uint32_t m_Width = 1600;
      uint32_t m_Height = 900;

  };
    
}
