
#include <glad/glad.h>
#include "ImGuiLayer.h"
#include "graphics/API.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <memory>
#include "entryLayer.h"

#include "iostream"

bool GV::EntryLayer::s_StopRendering = false;

GV::EntryLayer::EntryLayer(GV::API& api, bool& initApp)
  : m_Api(api), m_InitApp(initApp)
{
  glfwInit();
  //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
  glfwMakeContextCurrent(m_Window);
  //OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    //terminal -> printf("Failed to initialize GLAD\n");
    if (GLVersion.major < 4 || (GLVersion.major == 4 && GLVersion.minor < 5)); //terminal -> printf("It is required al least OpenGL version 4.5\n");
    throw std::runtime_error("Failed to initialize GLAD");
  }
  //vSync 
  glfwSwapInterval(1);

  glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
      {
	
	GV::EntryLayer::s_StopRendering = true;
      });

  getGPUInfo();
  start(); 
}

GV::EntryLayer::~EntryLayer()
{
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

void GV::EntryLayer::start()
{
  imGuiAttach();
  while (!s_StopRendering) 
  {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    imGuiBegin();
    showUI();
    imGuiEnd();

    glfwSwapBuffers(m_Window); 
  }
}

void GV::EntryLayer::imGuiAttach()
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
  //io.ConfigViewportsNoAutoMerge = true;
  //io.ConfigViewportsNoTaskBarIcon = true;

  float fontSize = 18.0f;
  io.Fonts -> AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
  io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  //SetDarkThemeColors();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
  ImGui_ImplOpenGL3_Init("#version 410");
}

void GV::EntryLayer::imGuiBegin()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void GV::EntryLayer::imGuiEnd()
{
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(getWidth(), getHeight());

  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void GV::EntryLayer::showUI()
{
  static bool dockspaceOpen = true;
  static bool opt_fullscreen_persistant = true;
  bool opt_fullscreen = opt_fullscreen_persistant;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
  // because it would be confusing to have two docking targets within each others.
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen)
  {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }
  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
  // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
  // all active windows docked into it will lose their parent and become undocked.
  // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
  // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", nullptr, window_flags);
  ImGui::PopStyleVar(); // end 0 padding for viewport

  if (opt_fullscreen) ImGui::PopStyleVar(2);
  
  // DockSpace
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 5.0f), dockspace_flags);
  }
  ImGui::SeparatorText("Select API");

  const char* apiItems[] = {"OpenGL", "Vulkan"};
  static int currentIdxApi = 0;
  const char* previewValue = apiItems[currentIdxApi];
  ImGui::SetCursorPosX(10.0f);
  if (ImGui::BeginCombo("API", previewValue))
  {
    for (int i = 0; i < IM_ARRAYSIZE(apiItems); i++)
    {
      const bool isSelected = (currentIdxApi == i);
      if (ImGui::Selectable(apiItems[i], isSelected)) currentIdxApi = i;
      if (isSelected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::SeparatorText("GPU Info");

  static ImGuiInputTextFlags flags = ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_ReadOnly;
  ImGui::SetCursorPosX(10.0f);
  ImGui::InputText(" OpenGL version", (char*)m_OpenGLVersion.c_str(), m_OpenGLVersion.size(), flags);
  ImGui::SetCursorPosX(10.0f);
  ImGui::InputText(" Vendor", (char*)m_Vendor.c_str(), m_Renderer.size(), flags);
  ImGui::SetCursorPosX(10.0f);
  ImGui::InputText(" Renderer", (char*)m_Renderer.c_str(), m_Renderer.size(), flags);

  ImGui::Dummy(ImVec2{0.0f, 20.0f});

  ImVec2 buttonSize = {150, 40};
  float width = ImGui::GetWindowSize().x;
  float centrePosition = (width - buttonSize.x) / 2;
  // tell Dear ImGui to render the button at the current y pos, but with the new x pos
  ImGui::SetCursorPosX(centrePosition);
  if (ImGui::Button("Run application", buttonSize))
  {
    switch (currentIdxApi) 
    {
      case 0:
	m_Api = GV::API::OPENGL; 
    }
    s_StopRendering = true;
    m_InitApp = true;
  }

  ImGui::End();
}

void GV::EntryLayer::getGPUInfo()
{
  const GLubyte *renderer = glGetString( GL_RENDERER ); 
  const GLubyte *vendor = glGetString( GL_VENDOR ); 
  const GLubyte *version = glGetString( GL_VERSION );

  m_OpenGLVersion = std::string(reinterpret_cast<const char*>(version));
  m_Vendor = std::string(reinterpret_cast<const char*>(vendor));
  m_Renderer = std::string(reinterpret_cast<const char*>(renderer));
}


