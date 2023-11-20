#include <chrono>

#include "ImGuiLayer.h"
#include "app.h"
#include "graphics/API.h"
#include "graphics/layer.h"
#include "graphics/renderer.h"
#include "graphics/shader.h"
#include "graphics/buffer.h"
#include "graphics/vertexArray.h"
#include "graphics/renderer.h"
#include "graphics/rendererAPI.h"
#include "graphics/texture.h"
#include "graphics/frameBuffer.h"
#include "graphics/window.h"
#include "io/reader.h"
#include "graphics/scene/mesh.h"
#include "entryLayer.h"
//#include "io/writer.h"

#include <iostream>
#include <memory>
#include <string>
#include <filesystem>

#include <GLFW/glfw3.h>

bool GV::App::s_appCreated = false;
bool GV::App::s_Running = true;
GV::App* GV::App::s_Instance = nullptr;

GV::App::App()
{
  m_Launcher = new EntryLayer(m_Api, m_InitApp);

  /*std::thread threadObject([]()
      {
	while (!s_LauncherClosed) 
	{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	std::cout<<s_LauncherClosed<<"\n";
	}
      });
  threadObject.join();*/
}

GV::App::~App()
{
}

void GV::App::startApp()
{
  // it is not longer necessary
  delete m_Launcher;
  s_Instance = this;

  // First create context
  m_RenderWindow = GV::Window::create(m_Api);

  GV::initReaderDrivers();

  GV::RendererAPI::setAPI(m_Api);
  GV::Renderer::init(m_Api);
  GV::Renderer::setRendererAPI();
   
  m_EditorLayer = new EditorLayer(m_Api);
  m_EditorLayer -> setGLFWwindow((GLFWwindow*)m_RenderWindow -> getWindow());
  pushOverlay(m_EditorLayer);

  m_ImGuiLayer = new ImGuiLayer();
  pushOverlay(m_ImGuiLayer);

  m_RenderWindow ->setCameraController(m_EditorLayer -> getCameraController());
  m_RenderWindow -> setGizmoOperation(m_EditorLayer -> getGizmoOperation());

  run();
}

void GV::App::run()
{
  while (s_Running) 
  {
    if (!m_Minimized)
    {
      for (Layer* layer : m_LayerStack) layer ->onUpdate();

      m_ImGuiLayer -> begin();
      for(Layer* layer : m_LayerStack) layer -> onImGuiRender();
      m_ImGuiLayer -> end();
      
      m_RenderWindow -> update();
    }
  }
}

void GV::App::pushLayer(Layer *layer)
{
  m_LayerStack.pushLayer(layer);
  layer -> onAttach();
}

void GV::App::pushOverlay(Layer *layer)
{
  m_LayerStack.pushOverlay(layer);
  layer -> onAttach();
}
