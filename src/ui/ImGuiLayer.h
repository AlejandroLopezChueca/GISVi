#pragma once

#include <cstdint>

#include "graphics/layer.h"


namespace GV 
{
  class ImGuiLayer : public Layer
  {
    public:
      ImGuiLayer();
      ~ImGuiLayer();

      void onAttach() override;
      void onDetach() override;
      void onImGuiRender() override;
      void onEvent() override;

      void begin();
      void end();

      void blockEvents(bool block) { m_BlockEvents = block; }
      
      void SetDarkThemeColors();

      uint32_t GetActiveWidgetID() const;

    private:
      // to block or not events that ImGui receive
      bool m_BlockEvents = true;
  };

}
