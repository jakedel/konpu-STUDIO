#include "al/ui/al_ControlGUI.hpp"

#include "../ui_fonts.cpp"
#include "../ui_helpers.cpp"

using namespace al;

void addMixerChannel(
  int height, int index,
  int *selection, float *volume, float *pan,
  ImU32 color,
  float meterLeft=0, float meterRight=0
) {
  ImVec2 cs = ImGui::GetCursorScreenPos();
  ImVec2 c = ImGui::GetCursorPos();

  bool selected = *selection == index;

  // Layout Constants
  const float TRACK_SIZE = 82;
  const float HEADER_HEIGHT = 32;
  const float PADDING = 8;
  const float CONTROL_PADDING = 6;
  // const float CONTROL_HEIGHT = 32;
  const float INPUT_HEIGHT = 24;

  const float CONTENT_WIDTH = TRACK_SIZE - PADDING*2;
  // const float FADER_HEIGHT = height - CONTROL_PADDING*2 - CONTROL_HEIGHT - 32 - HEADER_HEIGHT;
  const float FADER_HEIGHT = height - CONTROL_PADDING - 32 - HEADER_HEIGHT;
  const float METER_HEIGHT = FADER_HEIGHT - INPUT_HEIGHT - 4;

  // Layout Variables
  float PULL_TAB_HEIGHT = 4;
  ImU32 colorLeft = color;
  ImU32 colorRight = color;
  
  float left = meterLeft;
  float right = meterRight;
  if (left > 1) { left = 1; colorLeft = IM_COL32(255, 0, 0, 255); }
  if (right > 1) { right = 1; colorRight = IM_COL32(255, 0, 0, 255); }
  
  float pixelsLeft = left * METER_HEIGHT;
  float pixelsRight = right * METER_HEIGHT;
  
  /* Header */
  {
    ImGui::PushFont(fontLargeNumbers);
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.1, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, transparent(color, 0.875));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, transparent(color, 0.575));

    // Track Button
    {
      std::stringstream n; n << (index + 1) << "##track" << index << "_header";

      if (ImGui::Button(n.str().c_str(), ImVec2(TRACK_SIZE, HEADER_HEIGHT)))
      {
        *selection = index;
        selected = true;
      }

      if (selected) PULL_TAB_HEIGHT = 8;
      if (ImGui::IsItemActive()) PULL_TAB_HEIGHT = 6;
    }

    // Selection Highlight
    if (selected)
    {
      drawRect(cs, transparent(color, 0.125),
        0, HEADER_HEIGHT,
        TRACK_SIZE, height - HEADER_HEIGHT
      );
    }

    // Selection Indicator
    if (selected)
    {
      drawTri(cs, IM_COL32_WHITE,
        ImVec2(TRACK_SIZE, 0.f),
        ImVec2(TRACK_SIZE - HEADER_HEIGHT, 0.f),
        ImVec2(TRACK_SIZE, HEADER_HEIGHT)
      );
    }

    // Pull Tab
    {
      drawRect(cs, transparent(color, 0.75),
        0, PULL_TAB_HEIGHT * -1,
        TRACK_SIZE, PULL_TAB_HEIGHT
      );
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopFont();
  }

  ImGui::PushFont(fontNumbers);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);

  /* Faders */
  {
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 5);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, transparent(color, 0.375));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, transparent(color, 0.625));
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, color);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, IM_COL32_WHITE);
    
    // Pan Fader
    {
      xyw(c,
        PADDING,
        HEADER_HEIGHT + PADDING,
        CONTENT_WIDTH
      );

      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 1.5f));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, transparent(color, 0.375));
      
      std::stringstream n; n << "##track" << index << "_pan";
      ImGui::SliderFloat(n.str().c_str(), pan, -1.0f, 1.0f);

      ImGui::PopStyleColor();
      ImGui::PopStyleVar();
    }

    // Volume Fader
    {
      xy(c,
        PADDING,
        HEADER_HEIGHT + 32
      );

      // Use transparent background on selected track to show color
      if (selected)
      {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(20, 20, 20, 191));
      }
      else
      {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(20, 20, 20, 255));
      }

      std::stringstream n; n << "##track" << index << "_volume";
      ImGui::VSliderFloat(n.str().c_str(), ImVec2(28, FADER_HEIGHT), volume, 0.0f, 1.5f, "");
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
  }

  const float RIGHT_COL_START = TRACK_SIZE - PADDING - 31;

  /* Volume Input */
  {
    xyw(c,
      RIGHT_COL_START,
      64,
      31
    );

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3.5f, 5));

    std::stringstream n; n << "##track" << index << "_volume-input";
    ImGui::InputFloat(n.str().c_str(), volume, 0, 0, 2, ImGuiInputTextFlags_AutoSelectAll);
    
    ImGui::PopStyleVar();
  }

  ImGui::PopStyleVar();
  ImGui::PopFont();

  /* Meters */
  const float Y_START = HEADER_HEIGHT + 60;
  {
    // Left
    {
      // Draw background
      drawRect(cs, IM_COL32(32, 32, 32, 255),
        RIGHT_COL_START, Y_START,
        14, METER_HEIGHT,
        3
      );

      // Draw value
      if (pixelsLeft > 0)
      {
        drawRect(cs, colorLeft,
          RIGHT_COL_START, Y_START + METER_HEIGHT - pixelsLeft,
          14, pixelsLeft,
          3
        );
      }
    }

    // Right
    {
      // Draw background
      drawRect(cs, IM_COL32(32, 32, 32, 255),
        RIGHT_COL_START + 17, Y_START,
        14, METER_HEIGHT,
        3
      );

      // Draw value
      if (pixelsRight > 0)
      {
        drawRect(cs, colorRight,
          RIGHT_COL_START + 17, Y_START + METER_HEIGHT - pixelsRight,
          14, pixelsRight,
          3
        );
      }
    }
  }

  /* Mute Button */
  // {
  //   ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
  //   xy(c, 6, Y_START+METER_HEIGHT+6);
  //   ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(32, 32, 32, 255));
  //   ImGui::Button("MUTE", ImVec2(69, 32));
  //   ImGui::PopStyleVar();
  // }
    
  
  // Move cursor to next track
  xy(c, TRACK_SIZE, 0);
}
