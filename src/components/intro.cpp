#include <vector>

#include "al/ui/al_ControlGUI.hpp"

#include "../ui_fonts.cpp"
#include "../ui_helpers.cpp"

using namespace al;

const int LOG_SIZE = 5;

float ease(float value)
{
  if (value < 0) return 0;
  if (value > 1) return 1;
  
  if (value <= 0.5f)
  {
    return 2.0f * value * value;
  }

  value -= 0.5f;
  return 2.0f * value * (1.0f - value) + 0.5f;
}

bool addIntro(float screenWidth, float screenHeight, int progress, int size)
{
  bool button = false;
  float progressValue = (float)progress / (float)size;

  {
    windowColor(IM_COL32_WHITE);
    window("konpu-STUDIO intro.", ImVec2(289, 236), ImVec2(501, 296));
    ImVec2 c = ImGui::GetCursorPos();
    ImVec2 cs = ImGui::GetCursorScreenPos();

    ImU32 logoColor = transparent(IM_COL32_BLACK, ease(progressValue * 4.f));

    // Pattern
    drawRect(cs, IM_COL32(238, 238, 238, 255), 0, 0, 501, 19);
    drawRect(cs, IM_COL32(243, 133, 212, 255), 482, 19, 19, 19);
    drawRect(cs, IM_COL32(0, 0, 0, 255), 0, 258, 38, 38);

    // Progress Bar
    {
      ImGui::PushStyleColor(ImGuiCol_Text, transparent(IM_COL32_BLACK, ease(progressValue * 8.f)));
      ImGui::PushFont(fontDisplay);
      xy(c, 88, 145);
      ImGui::Text("Designed & developed by JAKE DELGADO");
      ImGui::SetCursorPosX(88);
      ImGui::Text("Intro to Allolib S22 Project.");
      ImGui::Text("");
      ImGui::SetCursorPosX(88);
      ImGui::PopStyleColor();
      
      ImGui::PushStyleColor(ImGuiCol_Text, transparent(IM_COL32(136, 136, 136, 255), ease(progressValue * 8.f) * 0.75f));
      ImGui::Text("https://github.com/jakedel/konpu-STUDIO");
      ImGui::PopStyleColor();
      ImGui::PopFont();
    }

    // KON-PU studio LOGO.
    {
      xy(c, 40, 63);
      cs = ImGui::GetCursorScreenPos();

      // Ripple
      drawCircleFill(cs, IM_COL32(196, 196, 196, 32), 24, 33, 16.f * ease(progressValue * 1.f));
      drawCircleFill(cs, IM_COL32(196, 196, 196, 32), 24, 33, 26.f * ease(progressValue * 1.625f));
      drawCircleFill(cs, IM_COL32(246, 141, 192, 64), 24, 33, 23.f * ease(progressValue * 1.375f));
      drawCircleFill(cs, IM_COL32(246, 141, 192, 13), 24, 33, 64.f * ease(progressValue * 2.375f));
      drawCircleFill(cs, IM_COL32(246, 141, 192, 13), 24, 33, 46.f * ease(progressValue * 2.f));

      drawCircleFill(cs, IM_COL32(255, 255, 255, 255), 24, 33, 7.f * ease(progressValue * 3.f));
      drawCircleFill(cs, IM_COL32(196, 196, 196, 255), 24, 33, 5.f * ease(progressValue * 2.625f));

      // Shape
      drawRect(cs, logoColor, 0, 0, 3, 45);
      drawLine(cs, logoColor, ImVec2(1.5f, 26), ImVec2(9.5f, 37));
      drawCircle(cs, logoColor, 24, 16, 7.f * ease(progressValue * 7.5f));
      drawCircle(cs, logoColor, 24, 33, 11.f * ease(progressValue * 5.f));
      drawRect(cs, logoColor, 22.5, 0, 3, 23.f * ease((progressValue * 5.f) - 0.5f));

      float barWidth = 22.f * ease((progressValue * 5.f) - 2.5f);
      drawRect(cs, logoColor, 24.f - (barWidth / 2), 4, barWidth, 3);
    }

    // KON-PU studio Wordmark.
    {
      xy(c, 89, 63);
      ImVec2 cs = ImGui::GetCursorScreenPos();
      c = ImGui::GetCursorPos();
      const float fadeFactor = 2;

      // 1 K
      drawRect(cs, logoColor, 0, 0, 2, 19);
      drawRect(cs, logoColor, 2, 6, 10, 2);
      drawRect(cs, logoColor, 6, 0, 2, 6);
      drawRect(cs, logoColor, 10, 8, 2, 11);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 1 o
      drawRect(cs, logoColor, 22, 6, 2, 13);
      drawRect(cs, logoColor, 32, 6, 2, 13);
      drawRect(cs, logoColor, 24, 6, 8, 2);
      drawRect(cs, logoColor, 24, 17, 8, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 1 n
      drawRect(cs, logoColor, 44, 6, 2, 13);
      drawRect(cs, logoColor, 54, 6, 2, 13);
      drawRect(cs, logoColor, 46, 6, 8, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // // 1 -
      // drawRect(cs, logoColor, 65, 10, 10, 2);
      // logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 1 p
      drawRect(cs, logoColor, 84-19, 6, 2, 20);
      drawRect(cs, logoColor, 94-19, 6, 2, 13);
      drawRect(cs, logoColor, 86-19, 6, 8, 2);
      drawRect(cs, logoColor, 86-19, 17, 8, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 1 u
      drawRect(cs, logoColor, 106-19, 6, 2, 13);
      drawRect(cs, logoColor, 116-19, 6, 2, 13);
      drawRect(cs, logoColor, 108-19, 17, 8, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 1 -
      drawRect(cs, logoColor, 109, 10, 10, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 2 S
      drawRect(cs, logoColor, 0, 26, 12, 2);
      drawRect(cs, logoColor, 0, 28, 2, 6);
      drawRect(cs, logoColor, 2, 32, 10, 2);
      drawRect(cs, logoColor, 10, 34, 2, 11);
      drawRect(cs, logoColor, 0, 43, 10, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));
      
      // 2 t
      drawRect(cs, logoColor, 27, 26, 2, 19);
      drawRect(cs, logoColor, 22, 32, 12, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 2 u
      drawRect(cs, logoColor, 44, 32, 2, 13);
      drawRect(cs, logoColor, 53, 32, 2, 13);
      drawRect(cs, logoColor, 46, 43, 8, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 2 d
      drawRect(cs, logoColor, 65, 32, 2, 13);
      drawRect(cs, logoColor, 75, 26, 2, 19);
      drawRect(cs, logoColor, 67, 32, 8, 2);
      drawRect(cs, logoColor, 67, 43, 8, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 2 i
      drawRect(cs, logoColor, 86, 26, 2, 2);
      drawRect(cs, logoColor, 86, 32, 2, 13);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));

      // 2 o
      drawRect(cs, logoColor, 98, 32, 2, 13);
      drawRect(cs, logoColor, 108, 32, 2, 13);
      drawRect(cs, logoColor, 100, 32, 8, 2);
      drawRect(cs, logoColor, 100, 43, 8, 2);
      logoColor = transparent(logoColor, ease(progressValue * fadeFactor));
    }

    // Version Mark
    {
      ImGui::PushFont(fontHeading);
      ImGui::PushStyleColor(ImGuiCol_Text, transparent(IM_COL32_BLACK, ease(progressValue * 1.f) * 0.5));
      xy(c, 120, 29);
      ImGui::Text("2023.06 Ver.");
      ImGui::PopStyleColor();
      ImGui::PopFont();

      ImGui::PopStyleColor();
    }

    ImGui::SetCursorPosX(501-64-8);
    ImGui::SetCursorPosY(296-32-8);
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(51, 51, 51, 255));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(136, 136, 136, 255));
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    button = ImGui::Button("Done", ImVec2(64, 32));
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
  }

  windowColor(IM_COL32(44, 44, 44, 255));
  window("konpu-STUDIO intro-background.", ImVec2(0, 0), ImVec2(screenWidth, screenHeight));

  return button;
}
