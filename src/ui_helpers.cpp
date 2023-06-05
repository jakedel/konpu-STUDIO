#include "al/ui/al_ControlGUI.hpp"

using namespace al;

#ifndef _UI_HELPERS_CPP
#define _UI_HELPERS_CPP

ImU32 color_mintto = IM_COL32(76, 210, 199, 255);

ImU32 colorForChannel(int channel)
{
  switch (channel % 9)
  {
    case 1:
      return IM_COL32(208, 181, 235, 255);
    case 2:
      return IM_COL32(247, 129, 186, 255);
    case 3:
      return IM_COL32(92, 211, 238, 255);
    case 4:
      return IM_COL32(173, 228, 54, 255);
    case 5:
      return IM_COL32(243, 133, 212, 255);
    case 6:
      return IM_COL32(189, 231, 175, 255);
    case 7:
      return IM_COL32(27, 208, 247, 255);
    case 8:
      return IM_COL32(228, 133, 243, 255);
    default:
      return color_mintto;
  }
}

void applySkin()
{
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_FrameBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(1.00f, 1.00f, 1.00f, 0.19f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(1.00f, 1.00f, 1.00f, 0.47f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.13f, 0.80f, 0.86f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 0.67f, 0.92f, 1.00f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.57f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.00f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Button]                 = ImVec4(1.00f, 1.00f, 1.00f, 0.34f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(1.00f, 1.00f, 1.00f, 0.57f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(1.00f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.38f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 0.67f, 0.92f, 0.25f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_Tab]                    = ImVec4(1.00f, 0.67f, 0.92f, 0.25f);
  colors[ImGuiCol_TabHovered]             = ImVec4(1.00f, 0.59f, 0.98f, 0.53f);
  colors[ImGuiCol_TabActive]              = ImVec4(1.00f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.00f, 0.67f, 0.92f, 0.25f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.59f, 0.98f, 1.00f);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 4));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
  ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 3.f);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
}

ImU32 transparent(ImU32 color, float factor)
{
  if (factor >= 1.0f)
  {
      return color;
  }
  
  ImU32 a = (color & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT;
  a = (ImU32)(a * factor);
  return (color & ~IM_COL32_A_MASK) | (a << IM_COL32_A_SHIFT);
}

void windowColor(ImU32 color)
{
  ImGui::PushStyleColor(ImGuiCol_WindowBg, color);
}

void window(const char *title, ImVec2 position, ImVec2 size)
{
  ImGui::SetNextWindowPos(position);
  ImGui::SetNextWindowSize(size);

  ImGui::Begin(title, NULL,
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoFocusOnAppearing |
    ImGuiWindowFlags_NoBringToFrontOnFocus
  );
}

bool addHeader(const char *title)
{
  bool isOpen = ImGui::CollapsingHeader(title, ImGuiTreeNodeFlags_DefaultOpen);
  return isOpen;
}

void xy(ImVec2 c, float x, float y)
{
  ImGui::SetCursorPosX(c.x + x);
  ImGui::SetCursorPosY(c.y + y);
}

void xyw(ImVec2 c, float x, float y, float w)
{
  ImGui::SetCursorPosX(c.x + x);
  ImGui::SetCursorPosY(c.y + y);
  ImGui::PushItemWidth(w);
}

void drawRect(ImVec2 c, ImU32 color, float x, float y, float w, float h, float round=0)
{
  ImGui::GetWindowDrawList()->AddRectFilled(
    ImVec2(c.x + x, c.y + y),
    ImVec2(c.x + x + w, c.y + y + h),
    color,
    round
  );
}

void drawRectIn(ImVec2 c, ImU32 color, float x1, float y1, float x2, float y2, float round=0)
{
  ImGui::GetWindowDrawList()->AddRectFilled(
    ImVec2(c.x + x1, c.y + y1),
    ImVec2(c.x + x2, c.y + y2),
    color,
    round
  );
}

void drawTri(ImVec2 c, ImU32 color, ImVec2 one, ImVec2 two, ImVec2 three)
{
  ImGui::GetWindowDrawList()->AddTriangleFilled(
    ImVec2(c.x + one.x, c.y + one.y),
    ImVec2(c.x + two.x, c.y + two.y),
    ImVec2(c.x + three.x, c.y + three.y),
    color
  );
}

void drawCircle(ImVec2 c, ImU32 color, float x, float y, float r, float thickness=2.5f)
{
  ImGui::GetWindowDrawList()->AddCircle(
    ImVec2(c.x + x, c.y + y),
    r,
    color,
    30,
    thickness
  );
}

void drawCircleFill(ImVec2 c, ImU32 color, float x, float y, float r, float thickness=2.5f)
{
  ImGui::GetWindowDrawList()->AddCircleFilled(
    ImVec2(c.x + x, c.y + y),
    r,
    color,
    30
  );
}

void drawLine(ImVec2 c, ImU32 color, ImVec2 one, ImVec2 two, float thickness=2.5f)
{
  ImGui::GetWindowDrawList()->AddLine(
    ImVec2(c.x + one.x, c.y + one.y),
    ImVec2(c.x + two.x, c.y + two.y),
    color,
    thickness
  );
}

#endif
