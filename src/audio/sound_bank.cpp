#include <vector>
#include "al/ui/al_ControlGUI.hpp"

#include "pcm_env.cpp"

using namespace al;

// 39 patches
std::vector<Patch*> SOUND_BANK = {
  new PatchListSeparator("FEATURED"),
  new Timbre(37, "Lo-Fi", "Lo-Fi Piano", {41, 46, 48, 57, 68}),
  new Timbre(31, "Essentials", "House Piano", {51, 63, 66, 70, 73}),
  new Timbre(8, "Future Bass", "Summer Breeze", {61,65,68,72,75,80,84,87}, 12),
  new Timbre(29, "Essentials", "Synthwave Chords", {51, 63, 66, 70, 73}, 12),
  new Timbre(4, "Essentials", "Marimba", {77,79,84,89,91,96,100}),
  new Timbre(10, "Essentials", "Reflection Guitar", {61,65,68,72}, 12),
  new Timbre(32, "Essentials", "Log Bass", {32, 39, 42, 44, 48, 49, 56, 58}, 12),
  new Timbre(30, "Essentials", "Wire Bass", {51, 63, 66, 70, 73}, 12),
  new Timbre(35, "Lo-Fi", "Electric Bass", {48, 60}),
  new Timbre(16, "Lo-Fi", "Dancing Strings", {65}),

  new DrumKit(33,
    "Drum Kit",

    std::vector<std::string> {
      "KICK-POP",
      "KICK-DAC",
      "KICK-SK",
      
      "SNARE-DAC",
      "CLAP-POP",

      "HAT-DANCE",
      "HAT-DAC",
      "HAT-SK",

      "SLAP-POP",
      "SLIDE-POP",
      "SLIDE-POP-OFFSET",
      
      "CRASH-LO",
      "CRASH-HI",
      "CRASH-HI",

      "SFX-BELL",
      "SFX-BOWL-LO",
      "SFX-BOWL-HI",
      "SFX-BREATH",
      "SFX-DROP-LO",
      "SFX-DROP-HI",
      "SFX-GOURD",
      "SFX-SHAKE-OFFSET",
      "SFX-SHAKE"
    },

    std::map<std::string, int> {
      {"CRASH-LO", 3},
      {"SFX-LOTUSBEND", -1},
      {"SFX-MK939", 0}
    }
  ),

  new PatchListSeparator("FUTURE BASS PACK"),
  new Timbre(1, "Future Bass", "Tokyo Lights", {84,88,96}),
  new Timbre(6, "Future Bass", "Fresh!", {65,67,72,79,84,89,96,101,103,105,106,108}),
  new Timbre(7, "Future Bass", "DECORATE", {87,89,91,92,94,96,99,101}),
  new Timbre(2, "Future Bass", "Spring emotion", {72,77,84}),
  new Timbre(3, "Future Bass", "Tempura", {62,65,69,72}),
  new Timbre(9, "Future Bass", "Sequence Saw", {65,68,72}),
  new Timbre(5, "Future Bass", "Square Synth", {60,65,67,69,72,77,79,84}),
  new Timbre(11, "Future Bass", "Mirai!", {61,65,68,72,75,80,84,87}),
  new Timbre(12, "Future Bass", "Summer Sweat", {61,65,68,72,75,80,84,87}, 12),
  new Timbre(18, "Future Bass", "SK-Pizzicato", {36, 48, 60, 72}, -14),
  new Timbre(13, "Future Bass", "Kokoponkon", {61,65,68,72,75,80,84,87}),

  new PatchListSeparator("LO-FI PACK"),
  new Timbre(14, "Lo-Fi", "YM-2800 DAC Piano", {41, 46, 48, 57, 68}),
  new Timbre(0, "Lo-Fi", "Tadpole Piano", {36, 45, 48, 60, 72, 84, 96}),
  new Timbre(36, "Lo-Fi", "MK-939 Piano", {60, 72}),
  new Timbre(17, "Lo-Fi", "Night Market Piano", {49, 61, 64, 68, 71, 75}, -14),
  new Timbre(15, "Lo-Fi", "Crossed Key Matrix", {84}),
  new Timbre(20, "Lo-Fi", "Out-Of-Tune Doorbell", {65, 82, 86}),
  new Timbre(28, "Lo-Fi", "Corroded Melody IC", {75}),
  new Timbre(23, "Lo-Fi", "Low Battery", {85}),
  new Timbre(22, "Lo-Fi", "Hivehole", {51, 63, 66, 70, 73}, 12),
  new Timbre(19, "Lo-Fi", "Enchanted Circuit", {65, 70}),
  new Timbre(27, "Lo-Fi", "Enchanted Wire", {75, 82, 85, 92}),
  new Timbre(21, "Lo-Fi", "Enchanted Lead", {60, 77, 78, 79, 80}),
  new Timbre(26, "Lo-Fi", "Unsettling Bass", {39, 42, 44, 48, 49, 51, 56, 57}, 12),
  new Timbre(34, "Lo-Fi", "PATCH34", {58, 61, 65, 68, 75, 80, 87}),
  new Timbre(24, "Lo-Fi", "Mystic Lead", {87, 94}),
  new Timbre(25, "Lo-Fi", "Sweet Dreams", {70, 85}),
};

std::map<int, Patch*> SOUND_BANK_MAP = {};

void makeSoundBankMap()
{
  for (int i = 0; i < SOUND_BANK.size(); i++)
  {
    Patch* patch = SOUND_BANK.at(i);
    SOUND_BANK_MAP[patch->id] = patch;
  }
}
