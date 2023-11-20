#pragma once

#include "imgui.h"
#include <string>

namespace GV
{
  enum class LogCategory
  {
    Info = 0, Warning, Error
  };

  class ImGuiLogPanel
  {
    public:
      ImGuiLogPanel();

      void clear();

      void onImGuiRender();

      static void addLog(const std::string& string, LogCategory category);

    private:
      static ImGuiTextBuffer s_Buf;
      static ImVector<unsigned int> s_LineOffsets; // INdex to lines offset. WE mantain thid with addLog() calls
      bool m_AutoScroll; // keep scrolling if already at the bottom
  };

}
