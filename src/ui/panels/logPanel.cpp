#include "logPanel.h"
#include "imgui.h"

#include <filesystem>

ImGuiTextBuffer GV::ImGuiLogPanel::s_Buf = ImGuiTextBuffer();
ImVector<unsigned int> GV::ImGuiLogPanel::s_LineOffsets = {};

GV::ImGuiLogPanel::ImGuiLogPanel()
{

}

void GV::ImGuiLogPanel::clear()
{
  s_Buf.clear();
  s_LineOffsets.clear();
  s_LineOffsets.push_back(0);
}

void GV::ImGuiLogPanel::onImGuiRender()
{
  ImGui::Begin("Log");
  bool clear = ImGui::Button("Clear");

  ImGuiListClipper clipper;
  clipper.Begin(s_LineOffsets.Size);
  const char* buf = s_Buf.begin();
  const char* buf_end = s_Buf.end();
  while (clipper.Step()) 
  {
    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
    {
      const char* line_start = buf + s_LineOffsets[line_no];
      const char* line_end = (line_no + 1 < s_LineOffsets.Size) ? (buf + s_LineOffsets[line_no + 1] - 1) : buf_end;
      ImGui::TextUnformatted(line_start, line_end);
    }
  }
  ImGui::End();
}

void GV::ImGuiLogPanel::addLog(const std::string& string, LogCategory category)
{
  std::string str;
  switch (category) 
  {
    case GV::LogCategory::Info:
      str = "[Info] ";
      str.append(string);
      break;
    case GV::LogCategory::Warning:
      str = "[Warning] ";
      str.append(string);
      break;
    case GV::LogCategory::Error:
      str = "[Error] ";
      str.append(string);
      break;
  }
  s_Buf.append(str.c_str());

  int oldSize = s_Buf.size();
  for (int newSize = s_Buf.size(); oldSize < newSize; oldSize++)
  {
    if(s_Buf[oldSize] == '\n') s_LineOffsets.push_back(oldSize + 1);
  }
}

