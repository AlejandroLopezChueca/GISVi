#pragma once

#include "graphics/API.h"
#include "graphics/scene/scene.h"

#include <filesystem>
#include <string>
#include <vector>

namespace GV
{
  

  class HECRASHDF5Panel
  {
    public:
      HECRASHDF5Panel(GV::API api);

      void onImGuiRender(bool* open);
      void setContext(GV::Scene* scene) {m_Context = scene;}
      void clear();

    private:
      void createSummaryDataset();
      void createTimeSeriesDataset();
      
      GV::API m_Api;
      GV::Scene* m_Context;

      int m_CurrentIdxDataset = 0;
      int m_CurrentIdxFlowArea = 0;
      int m_TypeDataset = 0;
      int m_TypeClip = 0;
      int m_CurrentIdxTerrain = 0;

      std::string m_PathGeometry;
      std::string m_PathDatasets;
      bool m_HasGeometryPathChanged = false;
      bool m_HasDatasetsPathChanged = false;
      
      std::vector<std::string> m_CellCoordinatesName;
      std::vector<std::string> m_CellCoordinatesPath;

      std::vector<std::string> m_SummaryDatasetsName;
      std::vector<std::string> m_TimeSeriesDatasetsName;
      std::vector<std::string> m_SummaryDatasetsPath;
      std::vector<std::string> m_TimeSeriesDatasetsPath;
  };

  class TerrainPanel
  {
    public:
      TerrainPanel(GV::API api);
      
      void onImGuiRender(bool* open);
      
      void setContext(GV::Scene* scene) {m_Context = scene;}

      void clear();

    private:

      std::filesystem::path m_PathTerrain;
      GV::API m_Api;
      
      GV::Scene* m_Context;
      
      int m_TypeRendering = 0;

  };

  class CubesFromPointsPanel
  {
    public:
      CubesFromPointsPanel(GV::API api);

      void onImGuiRender(bool* open);

      void setContext(GV::Scene* scene) {m_Context = scene;}

      void clear();


    private:
      void getAvailableTerrains();
      void createCubes();
      void createInstancedCubes();

      std::filesystem::path m_PathCubes;
      GV::API m_Api;

      std::string m_TerrainSelected;
      std::vector<std::string> m_AvailableTerrains;
      std::vector<entt::entity> m_EntityAvailableTerrains;
      std::vector<std::string> m_TypeDrawIntancing = {"Static", "Dynamic"};

      int m_TypeRendering = 0;
      int m_TypeInstancing = 0;

      int m_CurrentIdxTerrain = 0;
      int m_CurrentIdxTDrawInstancing = 0;

      float m_HeightCube = 0.0f;
      float m_XWidthCube = 0.0f;
      float m_ZWidthCube = 0.0f;

      GV::Scene* m_Context;
  };


}
