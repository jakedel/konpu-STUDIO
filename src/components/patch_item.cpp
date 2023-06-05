#include "al/ui/al_ControlGUI.hpp"

#include "../ui_fonts.cpp"
#include "../ui_helpers.cpp"

using namespace al;

bool addPatchItem(ImU32 color, float width, int index, int id, std::string name, bool selected)
{
  bool button = false;
  ImVec2 cs = ImGui::GetCursorScreenPos();
  ImVec2 c = ImGui::GetCursorPos();
  int height = 26;

  if (id == -1)
  {
    /*
      Render Patch List Separator
    */

    // Background
    {
      drawRect(cs, IM_COL32(100, 100, 100, 255),
        0, 0,
        width - 4, height
      );
    }

    // Label
    {
      ImGui::PushFont(fontPatchSubtitle);

      xy(c, 8, 3);
      ImGui::Text("%s", name.c_str());

      ImGui::PopFont();
    }
  }
  else
  {
    /*
      Render Patch
    */

    // Background
    {
      // ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

      if (selected)
      {
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
      }
      else
      {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(54, 54, 54, 255));
      }

      std::stringstream n; n << "##patch" << index;
      button = ImGui::Button(n.str().c_str(), ImVec2(width - 4, height));

      // Left Tab
      drawRect(cs, IM_COL32(0, 0, 0, 25),
        0, 0,
        31, height
      );

      if (selected) ImGui::PopStyleColor(); // pop additional selected state hover color
      ImGui::PopStyleColor();
      // ImGui::PopStyleColor();
    }

    // Index Label
    {
      if (selected && color == IM_COL32_WHITE)
      {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 127));
      }

      ImGui::PushFont(fontPatchNumbers);

      std::stringstream index_str;
      index_str << std::setw(2) << std::setfill('0') << index;

      xy(c, 8, 4);
      ImGui::Text("%s", index_str.str().c_str());

      ImGui::PopFont();
      
      if (selected && color == IM_COL32_WHITE)
      {
        ImGui::PopStyleColor();
      }
    }

    // Name Label
    {
      if (selected) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 127));
      ImGui::PushFont(fontPatchName);

      xy(c, 35, 4);
      ImGui::Text("%s", name.c_str());

      ImGui::PopFont();
      if (selected) ImGui::PopStyleColor();
    }
  }

  // Move to next item
  xy(c, 0, height);

  // Return if button clicked
  return button;
}
