#include <glad/glad.h>
#include "OpenGLWindow.h"

#include "graphics/renderer.h"
#include "ui/app.h"

#include "ImGuizmo.h"

#include <iostream>

bool GV::OpenGLWindow::s_GLFWInitialized = false;

GV::PerspectiveCameraController* GV::OpenGLWindow::s_cameraController = nullptr;

int* GV::OpenGLWindow::s_GizmoOperation = nullptr;

GV::OpenGLWindow::OpenGLWindow()
{
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
  setVSync(true);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_BLEND); 
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set GLFW callbacks
  glfwSetFramebufferSizeCallback(m_renderWindow, [](GLFWwindow* window, int width, int height) {glViewport(0, 0, width, height);});

  glfwSetWindowCloseCallback(m_renderWindow, [](GLFWwindow* window)
      {
	GV::App::close();
      });

  /*glfwSetMouseButtonCallback(m_renderWindow, [](GLFWwindow* window, int button, int action, int mods)
      {
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) std::cout<<"BUTTON"<<std::endl;
      });*/

  glfwSetKeyCallback(m_renderWindow, GV::OpenGLWindow::setKeyCallback);

  glfwSetScrollCallback(m_renderWindow, GV::OpenGLWindow::setScrollCallback);

  
#ifdef DEBUG_MODE
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  glEnable( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( debugMessageCallback, 0 );
#endif
  
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
  if (key == GLFW_KEY_1 && action == GLFW_PRESS)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  // Gizmos in editor Layer
  else if (key == GLFW_KEY_Q && action == GLFW_PRESS && !ImGuizmo::IsUsing())
  {
    *s_GizmoOperation = -1;
  }
  else if (key == GLFW_KEY_W && action == GLFW_PRESS && !ImGuizmo::IsUsing())
  {
    *s_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
  }
  else if (key == GLFW_KEY_E && action == GLFW_PRESS && !ImGuizmo::IsUsing())
  {
    *s_GizmoOperation = ImGuizmo::OPERATION::ROTATE;
  }
  else if (key == GLFW_KEY_R && action == GLFW_PRESS && !ImGuizmo::IsUsing())
  {
    *s_GizmoOperation = ImGuizmo::OPERATION::SCALE;
  }
}


void GV::OpenGLWindow::setScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
  s_cameraController -> onMouseScrolled(xOffset, yOffset);
}

void GV::OpenGLWindow::debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  if (type == GL_DEBUG_TYPE_ERROR) fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message );
}

