#include "al/ui/al_ControlGUI.hpp"

using namespace al;

#ifndef _UI_FONTS_CPP
#define _UI_FONTS_CPP

ImFont* fontUI;
ImFont* fontHeading;
ImFont* fontNumbers;
ImFont* fontPatchNumbers;
ImFont* fontLargeNumbers;
ImFont* fontParamValue;
ImFont* fontParamLabel;
ImFont* fontDisplay;
ImFont* fontPatchName;
ImFont* fontPatchSubtitle;

void loadFonts(float retinaScaleFactor)
{
  ImGuiIO& io = ImGui::GetIO();
  ImFontConfig config;

  // Adjust scale to fit fonts into less screen points, then load larger font sizes to compensate
  // This enables fonts to look clearer by using all pixels on high-res displays
  io.FontGlobalScale = 1 / retinaScaleFactor;

  config.GlyphExtraSpacing.x = 2;
  fontUI = io.Fonts->AddFontFromFileTTF(
    "res/Shinsen-Bold.otf",
    17 * retinaScaleFactor,
    &config
  );

  config.GlyphOffset.y = -1;
  config.GlyphExtraSpacing.x = 2.5f;
  fontPatchNumbers = io.Fonts->AddFontFromFileTTF(
    "res/Sauber.otf",
    20 * retinaScaleFactor,
    &config
  );

  config.GlyphOffset.y = 0.75;
  config.GlyphExtraSpacing.x = 3.25f;
  fontHeading = io.Fonts->AddFontFromFileTTF(
    "res/Shinsen-Bold.otf",
    18 * retinaScaleFactor,
    &config
  );

  config.GlyphOffset.y = 4;
  config.GlyphExtraSpacing.x = 0;
  fontLargeNumbers = io.Fonts->AddFontFromFileTTF(
    "res/Menulis.ttf",
    26 * retinaScaleFactor,
    &config
  );

  config.GlyphOffset.y = 0;
  config.GlyphExtraSpacing.x = 2;
  fontLargeNumbers = io.Fonts->AddFontFromFileTTF(
    "res/Menulis.ttf",
    33 * retinaScaleFactor, // 27
    &config
  );

  fontParamValue = io.Fonts->AddFontFromFileTTF(
    "res/Menulis.ttf",
    15 * retinaScaleFactor,
    &config
  );

  config.GlyphExtraSpacing.x = 6;
  config.GlyphOffset.y = -3;
  fontParamLabel = io.Fonts->AddFontFromFileTTF(
    "res/CenturyGothic.otf",
    10 * retinaScaleFactor,
    &config
  );

  config.GlyphExtraSpacing.x = 1;
  config.GlyphOffset.y = -1;
  fontNumbers = io.Fonts->AddFontFromFileTTF(
    "res/CenturyGothic.otf",
    15 * retinaScaleFactor,
    &config
  );

  config.GlyphExtraSpacing.x = -1.25f;
  config.GlyphOffset.y = -4;
  fontDisplay = io.Fonts->AddFontFromFileTTF(
    "res/CenturyGothic.otf",
    18.5f * retinaScaleFactor,
    &config
  );

  config.GlyphExtraSpacing.x = 0.5f;
  config.GlyphOffset.y = 0;
  fontPatchName = io.Fonts->AddFontFromFileTTF(
    "res/CenturyGothic.otf",
    17 * retinaScaleFactor,
    &config
  );

  config.GlyphExtraSpacing.x = -1.5f;
  config.GlyphOffset.y = 0;
  fontPatchSubtitle = io.Fonts->AddFontFromFileTTF(
    "res/CenturyGothic.otf",
    19 * retinaScaleFactor,
    &config
  );
}

#endif
