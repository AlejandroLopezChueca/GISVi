#pragma once

#include <functional>
#include <memory>

//#include "geometry/mesh.h"
//#include "io/reader.h"
#include "graphics/window.h"


namespace GV
{
  class App
  {
    public:
      App();
      ~App();

      void startApp();
      void run();


      static bool s_appCreated;


      //std::string pathFiles;
    private:
      std::unique_ptr<GV::Window> m_RenderWindow;
      //std::unique_ptr<FLB::OrthographicCameraController> m_OrthographicCameraController;
  };

}
