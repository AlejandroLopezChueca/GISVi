
#include "glm/fwd.hpp"
#include "graphics/API.h"
#include "graphics/frameBuffer.h"
#include "editorLayer.h"
#include "app.h"
#include "graphics/renderer.h"
#include "graphics/scene/components.h"
#include "graphics/scene/mesh.h"
#include "graphics/scene/scene.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/OpenGL/OpenGLUtils.h"
#include "io/reader.h"
#include "math/math.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include <iostream>
#include <memory>
#include <string>
#include "tinyfiledialogs.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <gdal/gdal_priv.h>
#include <glm/gtc/type_ptr.hpp>

GV::EditorLayer::EditorLayer(GV::API api)
  : m_Api(api), m_HierarchyPanel(api), m_ContentBrowserPanel(api), m_TerrainPanel(api), m_CubesFromPointsPanel(api), m_HECRASHDF5Panel(api)
{

}

void GV::EditorLayer::onUpdate()
{
  m_Scene -> resizeViewport(m_ViewportSize.x, m_ViewportSize.y);
  m_CameraController -> GLFWUpdate(m_GLFWwindow);
  FrameBufferSpecifications spec = m_Framebuffer -> getSpecifications();
  // resize frameBuffer
  if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&  // zero sized framebuffer is invalid
      (spec.width != m_ViewportSize.x || spec.height != m_ViewportSize.y))
  {
    m_Framebuffer-> resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    //m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
    m_CameraController -> setViewportSize(m_ViewportSize.x, m_ViewportSize.y);
  }

  // Render
  m_Framebuffer -> bind();
  GV::Renderer::setClearColor({0.5f,0.5f, 0.5f, 1});
  GV::Renderer::clear();

  GV::Renderer::beginScene(m_CameraController -> getCamera());
  m_Scene -> update(m_CameraController -> getCamera());

  //auto[mx, my] = ImGui::GetMousePos();

  m_Framebuffer -> unbind();
}

void GV::EditorLayer::onAttach()
{
  newProject();

  GV::FrameBufferSpecifications specs;
  specs.width = 1280;
  specs.height = 720;
  m_Framebuffer = GV::FrameBuffer::create(m_Api, specs);

  //m_IconGizmo = GV::Texture2D::create(m_Api, "resources/icons/gizmo.png");


  // TODO Temporal
  std::string path = "assets/MDT_rec_manz.tif";
  GV::BufferTypeDraw bufferTypeDrawTerr = GV::BufferTypeDraw::Static;
  auto terrain = m_Scene -> createEntity("Terrain");
  terrain.addComponent<GV::TerrainMeshComponent>(m_Api, GV::TypeTessellation::None, path, m_Scene -> m_ShiftScaleValues, bufferTypeDrawTerr, &terrain.getComponent<GV::DrawComponent>().draw);

  {
    auto& terrainComponent = terrain.getComponent<GV::TerrainMeshComponent>();
    auto& transformComponent = terrain.getComponent<GV::TransformComponent>();
    transformComponent.translation = {terrainComponent.mesh -> getMeanXCoordinate(), terrainComponent.mesh ->getMeanHeight(), terrainComponent.mesh -> getMeanZCoordinate()};
    transformComponent.originalTranslationValues = transformComponent.translation;
  }

  
  /*path = "assets/tee3.tif";
  auto terrain3 = m_Scene -> createEntity("Terrain3");
  terrain3.addComponent<GV::TerrainMeshComponent>(m_Api, GV::TypeTessellation::None, path, m_Scene -> m_ShiftScaleValues, &terrain3.getComponent<GV::DrawComponent>().draw);

  {
    auto& terrainComponent = terrain3.getComponent<GV::TerrainMeshComponent>();
    auto& transformComponent = terrain3.getComponent<GV::TransformComponent>();
    transformComponent.translation = {terrainComponent.mesh -> getMeanXCoordinate(), terrainComponent.mesh ->getMeanHeight(), terrainComponent.mesh -> getMeanZCoordinate()};
    transformComponent.originalTranslationValues = transformComponent.translation;
  }*/
  

  //path = "assets/Manztierra.p02.hdf";
  //GDALDataset* dataset = GV::RasterReader::openDataset(path);
  //GV::RasterReader::getHDF5Datasets(dataset);
  

  uint32_t widthRaster = terrain.getComponent<GV::TerrainMeshComponent>().mesh -> getRasterWidth();
  float rasterMeanHeight = terrain.getComponent<GV::TerrainMeshComponent>().mesh -> getMeanHeight();
  path = "assets/hh.gpkg";
  GV::BufferTypeDraw bufferTypeDraw = GV::BufferTypeDraw::Static;
  auto pilots = m_Scene -> createEntity("Pilots");


  GV::CubeMeshParameters parameters;
  parameters.api = m_Api;
  parameters.rasterWidth = widthRaster;
  parameters.rasterMeanHeight = rasterMeanHeight;
  parameters.xWidthCube = 10.0f;
  parameters.zWidthCube = 10.0f;
  parameters.heightCube = 50.0f;
  parameters.meshTypeCreation = GV::MeshTypeCreation::Instancing;
  parameters.bufferTypeDraw = GV::BufferTypeDraw::Static;
  parameters.typeInstancing = GV::TypeInstancing::MonoEntity;


  pilots.addComponent<GV::CubeMeshComponent>(path, terrain.getComponent<GV::TerrainMeshComponent>().mesh -> getHeightData(), terrain.getComponent<GV::TerrainMeshComponent>().mesh -> getGeotransform(), m_Scene -> m_ShiftScaleValues, parameters, &pilots.getComponent<GV::DrawComponent>().draw);
  {
    auto& cubeMeshComponent = pilots.getComponent<GV::CubeMeshComponent>();
    auto& transformComponent = pilots.getComponent<GV::TransformComponent>();
    transformComponent.translation = {cubeMeshComponent.mesh -> getXValueShift(), cubeMeshComponent.mesh ->getHeightValueShift(), cubeMeshComponent.mesh -> getZValueShift()};
    transformComponent.originalTranslationValues = transformComponent.translation;
  }

  m_CameraController = std::make_unique<GV::PerspectiveCameraController>(45.0f, 1.7778f, 0.1f, 16000.0f);
  m_SettingsPanel.setCameraController(m_CameraController.get());
  m_HierarchyPanel.setCameraController(m_CameraController.get());

}

void GV::EditorLayer::onDetach()
{

}

void GV::EditorLayer::onImGuiRender()
{
  // Note: Switch to true to enable dockspace
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
  ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
  ImGui::PopStyleVar(); // end 0 padding for viewport

  if (opt_fullscreen) ImGui::PopStyleVar(2);

  // DockSpace
  ImGuiIO& io = ImGui::GetIO();
  ImGuiStyle& style = ImGui::GetStyle();
  float minWinSizeX = style.WindowMinSize.x;
  style.WindowMinSize.x = 370.0f;
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }

  style.WindowMinSize.x = minWinSizeX;
  // Check visualization panels of the menuBar
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Exit")) GV::App::close();

      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View"))
    {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Setting"))
    {
      if (ImGui::MenuItem("General Settings")) m_ShowUISettings = true;
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Add"))
    {
      if (ImGui::MenuItem("Terrain") && !m_showUITerrain) m_showUITerrain = true; m_TerrainPanel.clear();
      if (ImGui::MenuItem("HEC-RAS HDF5") && !m_ShowUIHDF5) m_ShowUIHDF5 = true; m_HECRASHDF5Panel.clear();
      if (ImGui::BeginMenu("Cubes"))
      {
	if (ImGui::MenuItem("From Points") && !m_ShowUICubesFromPoints) m_ShowUICubesFromPoints = true; m_CubesFromPointsPanel.clear();
	if (ImGui::MenuItem("From Rectangles")) {;}
	ImGui::EndMenu();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Clip")) addClippingPlane();
      if (ImGui::MenuItem("Slice")) {}
      ImGui::Separator();
      if (ImGui::MenuItem("Light")) {}

      ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
  }

  // Show panels of the menuBar
  if (m_showUITerrain) UITerrain(&m_showUITerrain);
  if (m_ShowUIHDF5) UIHDF5(&m_ShowUIHDF5);
  if (m_ShowUICubesFromPoints) m_CubesFromPointsPanel.onImGuiRender(&m_ShowUICubesFromPoints);
  if (m_ShowUISettings) m_SettingsPanel.onImGuiRender(&m_ShowUISettings);

  // Viewport
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
  ImGui::Begin("Viewport");

  ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
  ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
  ImVec2 viewportOffset = ImGui::GetWindowPos();
  m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
  m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

  m_ViewportFocused = ImGui::IsWindowFocused();
  m_ViewportHovered = ImGui::IsWindowHovered();

  ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

  uint64_t textureID = m_Framebuffer -> getColorAttachmentID();
  ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0,1}, ImVec2{1,0});

  if (ImGui::BeginDragDropTarget())
  {
    ImGui::EndDragDropTarget();

  }

  // Gizmos
  GV::Entity& selectedEntity = m_HierarchyPanel.getSelectedEntity();
  if (selectedEntity && m_GizmoOperation != -1)
  {
    //ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    ImGui::GetWindowWidth();
    ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

    // Camera
    const glm::mat4& cameraProjection = m_CameraController -> getCamera().getProjectionMatrix();
    const glm::mat4& cameraView = m_CameraController -> getCamera().getViewMatrix();

    // Entiy transform
    auto& transformComponent = selectedEntity.getComponent<GV::TransformComponent>();
    glm::mat4 transform = transformComponent.getTransform();

    //ImGuizmo::DrawCubes(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(transform), 1);
    ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), (ImGuizmo::OPERATION)m_GizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(transform));

    if (ImGuizmo::IsUsing())
    {
      glm::vec3 translation, rotation, scale;
      GV::Math::decomposeTransform(transform, translation, rotation, scale);
      glm::vec3 deltaRotation = rotation -  transformComponent.rotation;
      transformComponent.translation = translation;
      transformComponent.rotation += deltaRotation;
      transformComponent.scale = scale;
    }
  }

  ImGui::End(); // end viewport
  ImGui::PopStyleVar(); 

  // Principal panels
  //UIToolBar();
  UILog();
  UILayersPanel();
  m_ContentBrowserPanel.onImGuiRender();

  ImGui::End(); // end dockspace
}

void GV::EditorLayer::newProject()
{
  m_Scene.reset(new GV::Scene());
  m_HierarchyPanel.setContext(m_Scene.get());
  m_TerrainPanel.setContext(m_Scene.get());
  m_CubesFromPointsPanel.setContext(m_Scene.get());
  m_HECRASHDF5Panel.setContext(m_Scene.get());
  Renderer::setRegistry(&m_Scene -> m_Registry);
}

void GV::EditorLayer::UIToolBar()
{
  ImGui::SetNextWindowSize(ImVec2(200, 50), ImGuiCond_FirstUseEver);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(50, 0));
  // make transparent area that are transparent
  static ImVec4 colorGizmoButton = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  auto& colors = ImGui::GetStyle().Colors;
  const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
  const auto& buttonActive = colors[ImGuiCol_ButtonActive];
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

  ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
  //ImGui::Checkbox("Gizmos", &m_UseGizmos);

  float size = ImGui::GetWindowHeight() - 4.0f;
  if (ImGui::ImageButton((ImTextureID)(uint64_t)m_IconGizmo->getID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, colorGizmoButton))
  { 
    m_UseGizmos = !m_UseGizmos;
    if (m_UseGizmos) colorGizmoButton = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);
    else colorGizmoButton = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  }
  ImGui::SetItemTooltip("Gizmo");

  ImGui::PopStyleVar(2);
  ImGui::PopStyleColor(3);
  ImGui::End();
}

void GV::EditorLayer::UILayersPanel()
{
  ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_FirstUseEver);
  m_HierarchyPanel.onImGuiRender();
}

void GV::EditorLayer::UILog()
{
  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
  m_LogPanel.onImGuiRender();
}

void GV::EditorLayer::UITerrain(bool *open)
{
  ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
  m_TerrainPanel.onImGuiRender(open);
}

void GV::EditorLayer::UIHDF5(bool* open)
{
  ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
  m_HECRASHDF5Panel.onImGuiRender(open);
}

void GV::EditorLayer::UICubesFromPoints(bool* open)
{

}

void GV::EditorLayer::addClippingPlane()
{
  // count number of existing clipping planes
  auto view = m_Scene -> m_Registry.view<GV::ClippingPlaneComponent>();
  int count = 0;
  for (auto entity : view) count += 1;

  // count max number of clipping planes
  // TODO. This has to be abstract
  int maxCountClipDistance = GV::OpenGLUtils::getMaxNumberClipDistance();

  if (count < maxCountClipDistance)
  {
    m_Scene -> m_CountClippingPlane = count + 1;
    GV::Renderer::setShadersToUse(count + 1);
    // TODO. This has to be abstract
    GV::OpenGLUtils::enableClipDistance(count);
    auto clippingPlane = m_Scene -> createEntity("Clipping Plane");
    clippingPlane.addComponent<GV::ClippingPlaneComponent>(&clippingPlane.getComponent<GV::DrawComponent>().draw);
    auto& transformComponent = clippingPlane.getComponent<GV::TransformComponent>();
    transformComponent.translation.y = m_Scene -> m_ShiftScaleValues.MeanHeight;
  }
  else 
  {
    std::string message = "The maximum number of clipping supported by the driver is " + std::to_string(maxCountClipDistance) + ".It is necessary to delete to add a new clipping plane";
    tinyfd_messageBox("Error", message.c_str(), "ok", "error", 1);
  }
}

