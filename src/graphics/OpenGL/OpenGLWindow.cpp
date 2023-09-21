#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "OpenGLWindow.h"

#include "graphics/renderer.h"

#include <iostream>

bool GV::OpenGLWindow::s_GLFWInitialized = false;

GV::PerspectiveCameraController* GV::OpenGLWindow::s_cameraController = nullptr;

GV::OpenGLWindow::OpenGLWindow(GV::PerspectiveCameraController* cameraController)
{
  s_cameraController = cameraController;
  if (!s_GLFWInitialized)
  {
    glfwInit();
    s_GLFWInitialized = true;
  }
  // Min version 4.5
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  m_renderWindow = glfwCreateWindow(m_Width, m_Height, title.c_str(), nullptr, nullptr);
  /*if (!m_renderWindow)
  {
    terminal -> printf("Window or context creation failed for OpenGL\n");
    throw std::runtime_error("Error in the creation of the window for OpenGL");
  }*/
  glfwMakeContextCurrent(m_renderWindow);
  //OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    //terminal -> printf("Failed to initialize GLAD\n");
    if (GLVersion.major < 4 || (GLVersion.major == 4 && GLVersion.minor < 5)); //terminal -> printf("It is required al least OpenGL version 4.5\n");
    throw std::runtime_error("Failed to initialize GLAD");
  }
  glViewport(0, 0, m_Width, m_Height);
  //glfwSetWindowUserPointer(renderWindow, &windowData);
  setVSync(true); 

  // Set GLFW callbacks
  glfwSetFramebufferSizeCallback(m_renderWindow, [](GLFWwindow* window, int width, int height) {glViewport(0, 0, width, height);});

  glfwSetWindowCloseCallback(m_renderWindow, [](GLFWwindow* window)
      {
	//WindowData& windowData = *(WindowData*)glfwGetWindowUserPointer(window);
	//windowData.isRendering = false;
	glfwSetWindowShouldClose(window,true);
      });

  glfwSetMouseButtonCallback(m_renderWindow, [](GLFWwindow* window, int button, int action, int mods)
      {
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) std::cout<<"BUTTON"<<std::endl;
      });

  glfwSetKeyCallback(m_renderWindow, GV::OpenGLWindow::setKeyCallback);
  /*glfwSetKeyCallback(m_renderWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)

      {
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) m_pauseRendering = !m_pauseRendering;
      });*/

  glfwSetScrollCallback(m_renderWindow, GV::OpenGLWindow::setScrollCallback);
  
}

GV::OpenGLWindow::~OpenGLWindow()
{
  glfwDestroyWindow(m_renderWindow);
  glfwTerminate();
}

void GV::OpenGLWindow::setVSync(bool enabled)
{
  if (enabled) glfwSwapInterval(1);
  else glfwSwapInterval(0);
}

void GV::OpenGLWindow::update()
{
  // Pool for and process events
  glfwPollEvents();
  // swap front and bahk buffers
  glfwSwapBuffers(m_renderWindow);
}

void GV::OpenGLWindow::setKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) GV::Renderer::s_UpdateRender = !GV::Renderer::s_UpdateRender;
  else if (GV::Renderer::s_VelocityPointMode && key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT)) GV::Renderer::s_PointSize += 0.2f;
  else if (GV::Renderer::s_VelocityPointMode && key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT)) GV::Renderer::s_PointSize -= 0.2f;
  else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
  {
    //if (GV::Renderer::s_VelocityPointMode) return;
    //GV::Renderer::s_VelocityPointMode = true;
    //GV::Renderer::s_PointSize = 2.0f;
    //GV::Renderer::s_shaderToUse = GV::Renderer::s_shaderPointsVelocity;
    //GV::Renderer::s_VelocityTextureMode = false;
  }
  else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
  {
    //GV::Renderer::s_VelocityPointMode = false;
    //GV::Renderer::s_shaderToUse = GV::Renderer::s_shaderTextureVelocity2D;
    //GV::Renderer::s_TextureToUse = GV::Renderer::s_TextureVelocity;
    //GV::Renderer::s_VelocityTextureMode = true;
  }
}


void GV::OpenGLWindow::setScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
  s_cameraController -> onMouseScrolled(xOffset, yOffset);
}

