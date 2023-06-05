#include "al/ui/al_ControlGUI.hpp"

#include "../ui_fonts.cpp"
#include "../ui_helpers.cpp"

using namespace al;

void addBoolParameter(float rowWidth, const char *name, bool *param, float width=98) {
  ImVec2 c = ImGui::GetCursorPos();

  if (c.x >= rowWidth) {
    c.x = 0;
    c.y += 31 + 1;
    ImGui::SetCursorPosX(c.x);
    ImGui::SetCursorPosY(c.y);
  }

  /*
    Parameter Value
  */
  {
    ImGui::PushFont(fontParamValue);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(32, 32, 32, 255));

    if (*param)
    {
      std::stringstream n; n << "ON" << "##param" << "_" << name;

      if (ImGui::Button(n.str().c_str(), ImVec2(width, 31)))
      {
        *param = false;
      }
    }
    else
    {
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 127));

      std::stringstream n; n << "OFF" << "##param" << "_" << name;

      if (ImGui::Button(n.str().c_str(), ImVec2(width, 31)))
      {
        *param = true;
      }
      
      ImGui::PopStyleColor();
    }
    
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopFont();
  }

  /*
    Parameter Label
  */
  {
    xy(c, 2, 2);
    ImGui::PushFont(fontParamLabel);
    ImGui::Text("%s", name);
    ImGui::PopFont();
  }

  xy(c, 99, 0);
}

void addParameter(
  float rowWidth,
  const char *name, float *param,
  float min, float max, float step,
  std::string type="float",
  float width=98
) {
  ImVec2 c = ImGui::GetCursorPos();

  if (c.x >= rowWidth) {
    c.x = 0;
    c.y += 31 + 1;
    ImGui::SetCursorPosX(c.x);
    ImGui::SetCursorPosY(c.y);
  }

  /*
    Parameter Value
  */
  {
    ImGui::SetNextItemWidth(width);
    ImGui::PushFont(fontParamValue);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 8));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(32, 32, 32, 255));

    std::stringstream n; n << "##param" << "_" << name;

    if (type == "hz")
    {
      if (*param == 20000) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 127));

      ImGui::DragFloat(n.str().c_str(), param, step, min, max, "%.0fHz", ImGuiSliderFlags_Logarithmic);

      if (*param == 20000) ImGui::PopStyleColor();
    }
    else
    {
      ImGui::DragFloat(n.str().c_str(), param, step, min, max, "%.2f");
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopFont();
  }

  /*
    Parameter Label
  */
  {
    xy(c, 2, 2);
    ImGui::PushFont(fontParamLabel);
    ImGui::Text("%s", name);
    ImGui::PopFont();
  }

  xy(c, 99, 0);
}
