#include "addLayersPanel.h"
#include "glm/fwd.hpp"
#include "graphics/API.h"
#include "graphics/buffer.h"
#include "graphics/scene/mesh.h"
#include "io/reader.h"
#include "graphics/scene/components.h"
#include "graphics/scene/entity.h"
#include "ui/uiUtils.h"

#include "imgui.h"
#include <array>
#include <filesystem>
#include <gdal/gdal_priv.h>
#include "tinyfiledialogs.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

//////////////////////////////// HECRASHDF5Panel ////////////////////////////////

GV::HECRASHDF5Panel::HECRASHDF5Panel(GV::API api) 
  : m_Api(api)
{

}

void GV::HECRASHDF5Panel::onImGuiRender(bool* open)
{
  ImGui::Begin("HEC-RAS HDF5", open);
  ImGui::SetCursorPosX(10.0f);
  ImGui::Text("Geometry path");
  ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
  ImGui::SetCursorPosX(10.0f);
  ImGui::InputText("##", (char*)m_PathGeometry.c_str(), m_PathGeometry.size());
  ImGui::SameLine();
  if (ImGui::Button("...##1"))
  {
    const char* list[1] = {"*.hdf"};
    const char* path = tinyfd_openFileDialog(NULL, NULL, 1, list, NULL, 0);
    if (path != NULL) 
    {
      m_PathGeometry = std::string(path);
      m_HasGeometryPathChanged = true;
    }
  }

  ImGui::SetCursorPosX(10.0f);
  ImGui::Text("Datasets path");
  ImGui::SetCursorPosX(10.0f);
  ImGui::InputText("##", (char*)m_PathDatasets.c_str(), m_PathDatasets.size());
  ImGui::SameLine();
  if (ImGui::Button("...##2"))
  {
    const char* list[1] = {"*.hdf"};
    const char* path = tinyfd_openFileDialog(NULL, NULL, 1, list, NULL, 0);
    if (path != NULL)
    {
      m_PathDatasets = std::string(path);
      m_HasDatasetsPathChanged = true;
    }
  }

  // Show geometries
  if (m_HasGeometryPathChanged)
  {
    if (std::filesystem::exists(m_PathGeometry))
    {
      try 
      {
	GDALDataset* dataset = GV::RasterReader::openDataset(m_PathGeometry);
	GV::RasterReader::getHDF5CellCoordinates(dataset, m_CellCoordinatesName, m_CellCoordinatesPath); 
      } 
      catch (...) 
      {
	tinyfd_messageBox("Error", "An error occurred while loading the Geometry", "ok", "error", 1);
	return; 
      }
    }
    else tinyfd_messageBox("Error", "The file of the geometry does not exists", "ok", "error", 1);
    
    m_HasGeometryPathChanged = false;
  }
  

  ImGui::Dummy(ImVec2{0.0f, 20.0f});
  ImGui::SeparatorText("Select Geometry");
  GV::UiUtils::drawComboBox(m_CellCoordinatesName, m_CurrentIdxFlowArea, "Geometry");

  // Show datasets
  if (m_HasDatasetsPathChanged)
  {
    if (std::filesystem::exists(m_PathDatasets))
    {
      try 
      {
	GDALDataset* dataset = GV::RasterReader::openDataset(m_PathDatasets);
	GV::RasterReader::getHDF5Datasets(dataset, m_SummaryDatasetsName, m_TimeSeriesDatasetsName, m_SummaryDatasetsPath, m_TimeSeriesDatasetsPath, m_CellCoordinatesName[m_CurrentIdxFlowArea]); 
      } 
      catch (...) 
      {
	tinyfd_messageBox("Error", "An error occurred while loading the Datasets", "ok", "error", 1);
	return; 
      }
    }
    else tinyfd_messageBox("Error", "The datasets file does not exists", "ok", "error", 1);
    
    m_HasDatasetsPathChanged = false;
  }

  ImGui::Dummy(ImVec2{0.0f, 20.0f});
  ImGui::SeparatorText("Select Dataset");

  ImGui::SetCursorPosX(10.0f);
  ImGui::RadioButton("Summary Dataset", &m_TypeDataset, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Time Series", &m_TypeDataset, 1); 
  ImGui::Dummy(ImVec2{0.0f, 10.0f});
  if (m_TypeDataset == 0) GV::UiUtils::drawComboBox(m_SummaryDatasetsName, m_CurrentIdxDataset, "Datasets");
  else if (m_TypeDataset == 1) GV::UiUtils::drawComboBox(m_TimeSeriesDatasetsName, m_CurrentIdxDataset, "Datasets");
  

  ImGui::Dummy(ImVec2{0.0f, 20.0f});
  ImGui::SeparatorText("Clip");
  ImGui::SetCursorPosX(10.0f);
  ImGui::RadioButton("None", &m_TypeClip, 0);
  ImGui::SameLine();
  ImGui::RadioButton("By Terrain Extension", &m_TypeClip, 1);

  // TODO Temporal
  std::vector<std::string> a = {"AAA", "BBB"};
  if (m_TypeClip == 1) GV::UiUtils::drawComboBox(a, m_CurrentIdxTerrain, "Terrains");

  ImGui::Dummy(ImVec2{0.0f, 40.0f});
  float width = ImGui::GetWindowSize().x;
  float height = ImGui::GetWindowSize().y;
  ImVec2 buttonSize = {40, 20};
  ImGui::SetCursorPos(ImVec2((width - 1.3f * buttonSize.x), (height - 1.8f * buttonSize.y)));
  if (ImGui::Button("Add", buttonSize))
  {
    switch (m_TypeDataset) 
    {
      // Summary Dataset
      case 0:
	if (m_SummaryDatasetsPath.size() > 0 && m_CellCoordinatesPath.size() > 0) createSummaryDataset(); break;
      // Time Series Dataset
      case 1:
	if (m_TimeSeriesDatasetsPath.size() > 0 && m_CellCoordinatesPath.size() > 0) createTimeSeriesDataset(); break;
    }
  } 
  ImGui::End();
}

void GV::HECRASHDF5Panel::clear()
{
  m_CurrentIdxDataset = 0;
  m_CurrentIdxFlowArea = 0;
  m_TypeDataset = 0;
  m_TypeClip = 0;
  m_CurrentIdxTerrain = 0;

  m_HasDatasetsPathChanged = false;
  m_HasGeometryPathChanged = false;

  m_PathGeometry.clear();
  m_PathDatasets.clear();

  m_CellCoordinatesName.clear();
  m_CellCoordinatesPath.clear();
  m_SummaryDatasetsName.clear();
  m_TimeSeriesDatasetsName.clear();
  m_SummaryDatasetsPath.clear();
  m_TimeSeriesDatasetsPath.clear();
}

void GV::HECRASHDF5Panel::createSummaryDataset()
{
  // Load coordinates mesh
  GDALDataset* coordinatesDataset = GV::RasterReader::openDataset(m_CellCoordinatesPath[m_CurrentIdxFlowArea]);
  //GV::RasterReader::loadHDF5Dataset(coordinatesDataset);
	  
  // Load dataset
  GDALDataset* summaryDataset = GV::RasterReader::openDataset(m_SummaryDatasetsPath[m_CurrentIdxDataset]);
  GV::RasterReader::loadHDF5Dataset(coordinatesDataset, summaryDataset);
  std::cout << m_SummaryDatasetsPath[m_CurrentIdxDataset]<< "\n";
  
  GV::BufferTypeDraw bufferTypeDraw = GV::BufferTypeDraw::Static;
  auto datasetEntity = m_Context -> createEntity(m_SummaryDatasetsName[m_CurrentIdxDataset]);


}

void GV::HECRASHDF5Panel::createTimeSeriesDataset()
{
  // Load dataset
  GDALDataset* timeDataset = GV::RasterReader::openDataset(m_TimeSeriesDatasetsPath[m_CurrentIdxDataset]);
  //GV::RasterReader::loadHDF5Dataset(timeDataset);
  std::cout << m_TimeSeriesDatasetsPath[m_CurrentIdxDataset]<< "\n";
}

//////////////////////////////// TerrainPanel ////////////////////////////////

GV::TerrainPanel::TerrainPanel(GV::API api)
  : m_Api(api)
{}

void GV::TerrainPanel::onImGuiRender(bool* open)
{
  ImGui::Begin("Terrain", open);
  ImGui::SetCursorPosX(10.0f);
  ImGui::Text("Terrain path");
  ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
  ImGui::SetCursorPosX(10.0f);
  ImGui::InputText("##", (char*)m_PathTerrain.c_str(), m_PathTerrain.string().size());
  ImGui::SameLine();
  if (ImGui::Button("...##1"))
  {
    const char* list[2] = {"*.tif", "*.tiff"};
    const char* path = tinyfd_openFileDialog(NULL, NULL, 1, list, NULL, 0);
    if (path != NULL) 
    {
      m_PathTerrain = std::filesystem::path(path);
    }
  }
  ImGui::Dummy(ImVec2{0.0f, 20.0f});
  ImGui::SeparatorText("Type Rendering");
  ImGui::SetCursorPosX(10.0f);
  ImGui::RadioButton("Basic", &m_TypeRendering, 0);
  ImGui::SameLine();
  ImGui::RadioButton("Uniform Tessellation", &m_TypeRendering, 1);
 
  ImGui::Dummy(ImVec2{0.0f, 20.0f});
  float width = ImGui::GetWindowSize().x;
  float height = ImGui::GetWindowSize().y;
  ImVec2 buttonSize = {40, 20};
  ImGui::SetCursorPos(ImVec2((width - 1.3f * buttonSize.x), (height - 1.8f * buttonSize.y)));
  if (ImGui::Button("Add", buttonSize))
  {
    GV::TypeTessellation typeTessellation;
    if (m_TypeRendering == 0) typeTessellation = GV::TypeTessellation::None;
    if (m_TypeRendering == 1) typeTessellation = GV::TypeTessellation::Uniform;
    if (std::filesystem::exists(m_PathTerrain))
    {
      GV::BufferTypeDraw bufferTypeDraw = GV::BufferTypeDraw::Static;
      auto terrain = m_Context -> createEntity(m_PathTerrain.filename());
      terrain.addComponent<GV::TerrainMeshComponent>(m_Api, typeTessellation, m_PathTerrain, m_Context -> m_ShiftScaleValues, bufferTypeDraw, &terrain.getComponent<GV::DrawComponent>().draw);

      // modify transform
      auto& terrainComponent = terrain.getComponent<GV::TerrainMeshComponent>();
      auto& transformComponent = terrain.getComponent<GV::TransformComponent>();
      transformComponent.translation = {terrainComponent.mesh -> getMeanXCoordinate(), terrainComponent.mesh ->getMeanHeight(), terrainComponent.mesh -> getMeanZCoordinate()};
      transformComponent.originalTranslationValues = transformComponent.translation;
    }

    else tinyfd_messageBox("Error", "The file does not exists", "ok", "error", 1);
  }
  ImGui::End();
}

void GV::TerrainPanel::clear()
{
  m_PathTerrain.clear();
  m_TypeRendering = 0;
}

//////////////////////////////// CubesFromPointsPanel ////////////////////////////////

GV::CubesFromPointsPanel::CubesFromPointsPanel(GV::API api)
  :m_Api(api)
{
}

void GV::CubesFromPointsPanel::onImGuiRender(bool *open)
{
  ImGui::SetNextWindowSize(ImVec2(200, 500), ImGuiCond_FirstUseEver);
  ImGui::Begin("Cubes from Points", open);
  
  ImGui::SetCursorPosX(10.0f);
  ImGui::Text("Cubes path");
  ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly;
  ImGui::SetCursorPosX(10.0f);
  ImGui::InputText("##", (char*)m_PathCubes.c_str(), m_PathCubes.string().size());
  ImGui::SameLine();
  if (ImGui::Button("...##1"))
  {
    const char* list[3] = {"*.gpkg", "*.shp", "*.dxf"};
    const char* path = tinyfd_openFileDialog(NULL, NULL, 1, list, NULL, 0);
    if (path != NULL) 
    {
      m_PathCubes = std::filesystem::path(path);
      getAvailableTerrains();
    }
  }
  // ComboBox
  ImGui::Dummy(ImVec2{0.0f, 10.0f});
  ImGui::SeparatorText("Terrain");
  GV::UiUtils::drawComboBox(m_AvailableTerrains, m_CurrentIdxTerrain, "Terrains");
  ImGui::SameLine();
  GV::UiUtils::helpMarker("Terrain for the extraction of heights for the points");

  // Dimensions cubes
  ImGui::Dummy(ImVec2{0.0f, 10.0f});
  ImGui::SeparatorText("Dimensions");
  ImGui::InputFloat("Width x direction", &m_XWidthCube, 0.01f, 1.0f, "%.3f");
  ImGui::InputFloat("Height cubes (y direction)", &m_HeightCube, 0.01f, 1.0f, "%.3f");
  ImGui::InputFloat("Width z direction", &m_ZWidthCube, 0.01f, 1.0f, "%.3f");

  // Type rendering
  ImGui::Dummy(ImVec2{0.0f, 20.0f});
  ImGui::SeparatorText("Type Rendering");
  ImGui::RadioButton("Batching", &m_TypeRendering, 0);
  ImGui::SameLine();
  GV::UiUtils::helpMarker("For static mesh, consume more memory than Instancing");
  ImGui::SameLine();
  ImGui::RadioButton("Instancing", &m_TypeRendering, 1); 

  if (m_TypeRendering == 1)
  {
    ImGui::Dummy(ImVec2{0.0f, 20.0f});
    ImGui::SeparatorText("Instancing options");
    ImGui::RadioButton("MonoEntity", &m_TypeInstancing, 0);
    ImGui::SameLine();
    ImGui::RadioButton("MultiEntity", &m_TypeInstancing, 1);
    GV::UiUtils::drawComboBox(m_TypeDrawIntancing, m_CurrentIdxTDrawInstancing, "Type Draw");
  }

  ImGui::Dummy(ImVec2{0.0f, 20.0f});
  float width = ImGui::GetWindowSize().x;
  float height = ImGui::GetWindowSize().y;
  ImVec2 buttonSize = {40, 20};
  ImGui::SetCursorPos(ImVec2((width - 1.3f * buttonSize.x), (height - 1.8f * buttonSize.y)));
  if (ImGui::Button("Add", buttonSize))
  {
    if (std::filesystem::exists(m_PathCubes)) createCubes();
    else tinyfd_messageBox("Error", "The file does not exists", "ok", "error", 1);
  } 
  ImGui::End();
}

void GV::CubesFromPointsPanel::clear()
{
  m_PathCubes.clear();
  m_TerrainSelected.clear();
  m_AvailableTerrains.clear();
  m_EntityAvailableTerrains.clear();

  m_TypeRendering = 0;
  m_CurrentIdxTerrain = 0;

  m_HeightCube = 0.0f;
  m_XWidthCube = 0.0f;
  m_ZWidthCube = 0.0f;

}

void GV::CubesFromPointsPanel::getAvailableTerrains()
{
  auto view = m_Context -> m_Registry.view<GV::TagComponent, GV::TerrainMeshComponent>();
  for (auto entity : view)
  {
    auto [tagComponent, terrainComponent] = view.get<GV::TagComponent, GV::TerrainMeshComponent>(entity);
    m_AvailableTerrains.push_back(tagComponent.tag);
    m_EntityAvailableTerrains.push_back(entity);
  }
}

void GV::CubesFromPointsPanel::createCubes()
{
  // Info of the terrain
  auto& terrainMeshComponent = m_Context -> m_Registry.get<GV::TerrainMeshComponent>(m_EntityAvailableTerrains[m_CurrentIdxTerrain]);
  uint32_t widthRaster = terrainMeshComponent.mesh -> getRasterWidth();
  float rasterMeanHeight = terrainMeshComponent.mesh -> getMeanHeight();
  const std::vector<float>& heightDataRaster = terrainMeshComponent.mesh -> getHeightData();
  const std::array<double, 6>& geotransform = terrainMeshComponent.mesh -> getGeotransform();
  
  // parameters to construct the mesh
  GV::CubeMeshParameters parameters;
  parameters.api = m_Api;
  parameters.rasterWidth = widthRaster;
  parameters.rasterMeanHeight = rasterMeanHeight;
  parameters.xWidthCube = m_XWidthCube;
  parameters.zWidthCube = m_ZWidthCube;
  parameters.heightCube = m_HeightCube;
  if (m_TypeRendering == 0) parameters.meshTypeCreation = GV::MeshTypeCreation::Batching;
  else if (m_TypeRendering == 1) parameters.meshTypeCreation = GV::MeshTypeCreation::Instancing;
  if (m_CurrentIdxTDrawInstancing == 0) parameters.bufferTypeDraw = GV::BufferTypeDraw::Static;
  else if (m_CurrentIdxTDrawInstancing == 1) parameters.bufferTypeDraw = GV::BufferTypeDraw::Dynamic;
  if (m_TypeInstancing == 0) parameters.typeInstancing = GV::TypeInstancing::MonoEntity;
  else if (m_TypeInstancing == 1) parameters.typeInstancing = GV::TypeInstancing::MultiEntity;
  
  if (parameters.typeInstancing == GV::TypeInstancing::MonoEntity || parameters.meshTypeCreation == GV::MeshTypeCreation::Batching)
  {
    // create cubes
    auto cubes = m_Context -> createEntity(m_PathCubes.filename());

    cubes.addComponent<GV::CubeMeshComponent>(m_PathCubes, heightDataRaster, geotransform, m_Context -> m_ShiftScaleValues, parameters, &cubes.getComponent<GV::DrawComponent>().draw);
	
    // Corrrect transform of the cubes
    auto& cubeMeshComponent = cubes.getComponent<GV::CubeMeshComponent>();
    auto& transformComponent = cubes.getComponent<GV::TransformComponent>();
    transformComponent.translation = {cubeMeshComponent.mesh -> getXValueShift(), cubeMeshComponent.mesh ->getHeightValueShift(), cubeMeshComponent.mesh -> getZValueShift()};
    transformComponent.originalTranslationValues = transformComponent.translation;
  }

  else 
  {
    // get number of objects for the instancing if it is used
    GV::VectorReader::CountTypesGeometry countTypesGeometry;
    GV::VectorReader::getCountGeometry(m_PathCubes, countTypesGeometry);

    // entity group
    auto cubesGroup = m_Context -> createEntity(m_PathCubes.stem());
    cubesGroup.addComponent<GV::ChildEntitiesComponent>(m_Context);

    auto& childEntities = cubesGroup.getComponent<GV::ChildEntitiesComponent>().childEntities;
    auto& parentComponent = cubesGroup.getComponent<GV::ParentComponent>();
    parentComponent.hasChilds = true;
    
    cubesGroup.addComponent<GV::CubeMeshComponent>(m_PathCubes, heightDataRaster, geotransform, m_Context -> m_ShiftScaleValues, parameters, &cubesGroup.getComponent<GV::DrawComponent>().draw);
    auto& cubeMeshComponent = cubesGroup.getComponent<GV::CubeMeshComponent>();

    std::vector<glm::vec3>& translationDataMesh = cubeMeshComponent.mesh -> getTranslationData(); 

    for (int i = 0; i < countTypesGeometry.countPoint; i++)
    {
      std::string name = m_PathCubes.stem().string() + std::to_string(i);
      //auto cube = m_Context -> createEntity(name, false);
      childEntities.push_back(m_Context -> createEntity(name, false));
      
      auto& transformComponent = childEntities[i].getComponent<GV::TransformComponent>();
      transformComponent.translation = translationDataMesh[i];
      transformComponent.originalTranslationValues = transformComponent.translation;
    } 
  }
}

void GV::CubesFromPointsPanel::createInstancedCubes()
{

}
