#include "al/ui/al_ControlGUI.hpp"

#include "../ui_fonts.cpp"
#include "../ui_helpers.cpp"

using namespace al;

bool addPatternItem(int index, std::string id, std::string name, bool selected, int size=64)
{
  bool button;
  ImVec2 c = ImGui::GetCursorPos();

  /*
    Background
  */
  {
    std::stringstream n; n << (index + 1) << "##pattern" << id;

    ImGui::PushFont(fontLargeNumbers);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.1, 0));

    if (selected)
    {
      ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(246, 141, 192, 255));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(246, 141, 192, 255));
    }
    else
    {
      ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(73, 73, 73, 255));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(100, 100, 100, 255));
    }

    button = ImGui::Button(n.str().c_str(), ImVec2(74, 46));

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopFont();
  }

  // // Name Label
  // {
  //   if (selected) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 127));
  //   ImGui::PushFont(fontPatchName);

  //   xy(c, 35, 19);
  //   ImGui::Text("%s", name.c_str());

  //   ImGui::PopFont();
  //   if (selected) ImGui::PopStyleColor();
  // }

  // Move to next item
  xy(c, 0, 47);

  // Return if button clicked
  return button;
}
