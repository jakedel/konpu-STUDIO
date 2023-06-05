#include <random>

#include "al/ui/al_ControlGUI.hpp"

// Cereal
#include "cereal/cereal.hpp"

// Constants
const float HIGHEST_NOTE = 127;
const float GRID_MARGIN_H = 8;
const float GRID_MARGIN_V = 8;
const float GRID_BASE_DIVISION = 16.f;
const float CELL_GAP = 1;

struct Note
{
  float pitch;
  float time;
  float duration;
  std::string id;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar(cereal::make_nvp("id", id), cereal::make_nvp("pitch", pitch), cereal::make_nvp("time", time), cereal::make_nvp("duration", duration));
  }
};

struct Pattern
{
  float length;
  std::string name;
  std::string id;
  std::vector<std::vector<Note>> channels;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar(cereal::make_nvp("length", length), cereal::make_nvp("name", name), cereal::make_nvp("id", id), cereal::make_nvp("tracks", channels));
  }
};

struct Channel
{
  int patch;
  float attack;
  float release;
  float volume;
  float pan;
  float lp;
  float tune;
  bool soft;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar(cereal::make_nvp("patch", patch), cereal::make_nvp("atk", attack), cereal::make_nvp("rel", release), cereal::make_nvp("volume", volume), cereal::make_nvp("pan", pan), cereal::make_nvp("lp", lp), cereal::make_nvp("tune", tune), cereal::make_nvp("soft", soft));
  }
};

struct Song
{
  std::string filename;
  float bpm;
  std::vector<Channel> channels;
  std::vector<Pattern> patterns;

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar(cereal::make_nvp("bpm", bpm), cereal::make_nvp("patterns", patterns), cereal::make_nvp("channels", channels));
  }
};


/* Level of abstraction: */
// 1. Time      (1 = a measure)
// 2. Note      (Uses time)
// 3. Div       (division=32: 0, 32, 64, 96)
// 4. Base Grid (division=32: 0, 0.5, 1, 1.5)
// 5. Cell Grid (division=32: 0, 1, 2, 3)
// 6. Screen


/* Time conversions */

float timeToCellX(float division, float time)
{
  return time * division;
}

float timeToBaseX(float time)
{
  return time * GRID_BASE_DIVISION;
}

float timeToDiv(float division, float time)
{
  return time * pow(division, 2);
}


/* Note conversions */

float pitchToY(float pitch)
{
  return HIGHEST_NOTE - pitch;
}

ImVec4 noteToDiv(float division, Note note)
{
  return ImVec4(
    timeToDiv(division, note.time),
    note.pitch,
    1,
    timeToDiv(division, note.duration)
  );
}

ImVec4 noteToBase(Note note)
{
  return ImVec4(
    timeToBaseX(note.time),
    pitchToY(note.pitch),
    1,
    timeToBaseX(note.duration)
  );
}

ImVec4 noteToCell(float division, Note note)
{
  return ImVec4(
    timeToCellX(division, note.time),
    pitchToY(note.pitch),
    1,
    timeToCellX(division, note.duration)
  );
}


/* Div conversions */

float divToTime(float division, float div)
{
  return div / division / division;
}

float divToBaseX(float division, float div)
{
  return (div / division) / (division / GRID_BASE_DIVISION);
}

ImVec4 divToBase(float division, float div, float pitch)
{
  return ImVec4(
    divToBaseX(division, div),
    pitchToY(pitch),
    1,
    GRID_BASE_DIVISION / division
  );
}


/* Base grid conversions */

float yToPitch(float y)
{
  return HIGHEST_NOTE - y;
}

float baseXToTime(float x)
{
  return x / GRID_BASE_DIVISION;
}

float baseXToDiv(float division, float x)
{
  return timeToDiv(division, baseXToTime(x));
}

float baseXToCellX(float division, float x)
{
  return timeToCellX(division, x);
}

ImVec4 baseFloor(float division, ImVec4 base)
{
  float factor = division / GRID_BASE_DIVISION;

  return ImVec4(
    floor(base.x * factor) / factor,
    floor(base.y),
    base.z,
    base.w
  );
}

ImVec4 baseToScreen(ImVec4 base, float baseWidth, float baseHeight, float gap, float scrollX = 0, float scrollY = 0)
{
  return ImVec4(
    ((GRID_MARGIN_H + base.x) * baseWidth) - scrollX,
    ((GRID_MARGIN_V + base.y) * baseHeight) - scrollY,
    (base.z * baseHeight) - gap,
    (base.w * baseWidth) - gap
  );
}

ImVec4 baseToDiv(float division, ImVec4 base)
{
  return ImVec4(
    baseXToDiv(division, base.x),
    yToPitch(base.y),
    1,
    baseXToDiv(division, base.w)
  );
}

ImVec4 baseToDivFloor(float division, ImVec4 base)
{
  ImVec4 div = baseToDiv(division, base);
  div.x = floor(div.x / division) * division;
  div.y = ceil(div.y);
  div.w = floor(div.w / division) * division;
  return div;
}

ImVec4 baseToCell(float division, ImVec4 base)
{
  return ImVec4(
    baseXToCellX(division, base.x),
    base.y,
    1,
    baseXToCellX(division, base.w)
  );
}


/* Cell grid conversions */

float cellXToDiv(float division, float x)
{
  return x * division;
}

float cellXToTime(float division, float x)
{
  return x / division;
}

float cellXToBaseX(float division, float x)
{
  return timeToBaseX(x / division);
}


/* Screen conversions */

ImVec4 screenToBase(ImVec2 screen, float baseWidth, float baseHeight, float scrollX = 0, float scrollY = 0)
{
  return ImVec4(
    ((screen.x + scrollX) / baseWidth) - GRID_MARGIN_H,
    ((screen.y + scrollY) / baseHeight) - GRID_MARGIN_V,
    1,
    0
  );
}

float getScreenWidth(float baseWidth, float baseCount)
{
  return (baseCount + (GRID_MARGIN_H * 2)) * baseWidth;
}

float getScreenHeight(float baseHeight)
{
  return (HIGHEST_NOTE + 1 + (GRID_MARGIN_V * 2)) * baseHeight;
}

std::string generateUuid() {
  static std::random_device dev;
  static std::mt19937 rng(dev());

  std::uniform_int_distribution<int> dist(0, 15);

  const char *v = "0123456789abcdef";
  const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

  std::string res;
  for (int i = 0; i < 16; i++) {
      if (dash[i]) res += "-";
      res += v[dist(rng)];
      res += v[dist(rng)];
  }
  return res;
}
