#include <iostream>
#include <cstdio>
#include <vector>
#include <set>

// Gamma
#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"
#include "Gamma/Oscillator.h"

// Allolib
#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al/scene/al_PolySynth.hpp"
#include "al/ui/al_ControlGUI.hpp"
#include "al/ui/al_Parameter.hpp"

// Cereal
#include "cereal/cereal.hpp"
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>

// GUI
#include "ui_fonts.cpp"
#include "ui_helpers.cpp"

// Editor
#include "editor_helpers.cpp"

// PCM Synthesizer
#include "audio/sound_bank.cpp"
#include "audio/pcm_env.cpp"

// Components
#include "components/intro.cpp"
#include "components/mixer_channel.cpp"
#include "components/parameter.cpp"
#include "components/patch_item.cpp"
#include "components/pattern_item.cpp"

using namespace al;

/* konpu ! */

class KonpuStudio : public App
{
public:
  // Audio components
  SynthGUIManager<PCMEnv> synthManager{"PCMEnv"}; // Custom lo-fi sample synthesizer

  // Layout constants
  const int MIXER_TAB_HEIGHT = 8;

  // Sample loading
  int progress = 0;
  int progressPatch = 0;
  int progressSample = 0;
  int progressSize = 0;

  // Musical typing
  int octaveShift = -1;

  // Pass through keyboard state
  std::set<std::string> modifiers = {};
  bool shouldZoomIn = false;
  bool shouldZoomOut = false;

  // Piano roll zoom
  float division = 16;
  float baseWidth = 25.f;
  float baseHeight = 20.f;

  // Modals
  bool modalOpen = false;
  bool aboutOpen = false;
  int aboutProgress = 0;
  std::string modalKind = "???";

  // Piano roll state
  int currentChannel = 0;
  int currentPattern = 0;
  bool playing = false;
  bool mixerVisible = true;
  bool allChannelsOpaque = false;
  float bpm = 120;
  float playhead = 0;
  bool shouldScrollToCenter = true;
  bool creatingNewNotes = false;
  bool addingNotes = false;
  bool resizingNotes = false;
  bool justManipulatedNote = false;
  bool noteAlreadyAtCursor = false;
  Note newNotesStart = {-1, -1, -1};
  Note newNotesMax = {-1, -1, -1};
  Song song;
  std::vector<Note> newNotes = {};
  std::set<float> notePreviews = {};
  std::set<int> playingNotes = {};
  float mouseMoveAmount = 0;
  int flipflop = 1;

  // Used for alternating the way that preview notes are played so the previous preview note on a channel can end at the same time the next note starts
  void flipTheFlop()
  {
    if (flipflop == 1)
    {
      flipflop = 2;
      return;
    }

    flipflop = 1;
  }

  void setParam(std::string name, float value)
  {
    synthManager.voice()->setInternalParameterValue(name, value);
  }

  void triggerOn(int channel, int id)
  {
    Channel params = song.channels.at(channel);
    synthManager.voice()->setInternalParameterValue("attackTime", params.attack);
    synthManager.voice()->setInternalParameterValue("releaseTime", params.release);
    synthManager.voice()->setInternalParameterValue("tune", params.tune);
    synthManager.voice()->setInternalParameterValue("interpolate", params.soft);
    synthManager.voice()->setInternalParameterValue("patch", params.patch);
    synthManager.voice()->setInternalParameterValue("amplitude", params.volume);
    synthManager.voice()->setInternalParameterValue("lp", params.lp);
    synthManager.triggerOn(id);
  }

  void triggerOff(int id)
  {
    synthManager.triggerOff(id);
  }

  void createNewPattern()
  {
    Pattern pattern = Pattern();
    pattern.name = "";
    pattern.length = 4;
    pattern.id = generateUuid();

    for (int channel = 0; channel < NUM_CHANNELS; channel++)
    {
      pattern.channels.push_back(std::vector<Note>());
    }

    song.patterns.push_back(pattern);
  }

  /* init */
  void onCreate() override
  {
    gam::sampleRate(audioIO().framesPerSecond()); // Set Gamma sample rate
    makeSoundBankMap();
    PCMEnv::SoundBankMap = &SOUND_BANK_MAP;
    imguiInit();
    loadFonts(this->highresFactor());
    applySkin();

    // Set up song
    song.bpm = 120;
    song.channels = std::vector<Channel>();
    song.patterns = std::vector<Pattern>();

    for (int i = 0; i < NUM_CHANNELS; i++)
    {
      Channel channel = Channel();
      channel.patch = 37;
      channel.attack = 0.001f;
      channel.release = 0.13f;
      channel.volume = 1.f;
      channel.pan = 0.f;
      channel.tune = 0.f;
      channel.lp = 20000.f;
      channel.soft = true;
      song.channels.push_back(channel);
    }

    createNewPattern();

    for (int i = 0; i < SOUND_BANK.size(); i++)
    {
      progressSize += SOUND_BANK.at(i)->i_size - 1;
    }

    // Load timbre & drum kit samples
    while (progress < progressSize)
    {
      if (SOUND_BANK.at(progressPatch)->load())
      {
        progressPatch++;
        progressSample = 0;
      }
      else
      {
        progressSample++;
        progress++;
      }
    }

    // Improve performance by using manual polyphony, which creates all voices in advance and prevents new voices from having to be created every single time a note is played
    synthManager.synth().disableAllocation<PCMEnv>();
    synthManager.synth().allocatePolyphony<PCMEnv>(256);
  }

  void onSound(AudioIOData &io) override
  {
    synthManager.render(io);
  }

  /* Keyboard Handling (musical typing) */
  bool onKeyDown(Keyboard const &k) override
  {
    if (k.alt())
    {
      modifiers.emplace("chord");
    }

    if (k.shift())
    {
      modifiers.emplace("zoom vertical");
    }

    if (k.key() == '`')
    {
      modifiers.emplace("double");
    }

    if (k.key() == '1')
    {
      modifiers.emplace("triplet");
    }

    /*
      Keybindings
    */
    if (progress == progressSize)
    {
      if (k.key() == '4')
      {
        mixerVisible = !mixerVisible;
        return true;
      }

      if (k.key() == '8')
      {
        allChannelsOpaque = !allChannelsOpaque;
        return true;
      }

      if (k.key() == ']')
      {
        // Musical typing octave down
        octaveShift--;
        return true;
      }
      else if (k.key() == '\\')
      {
        // Musical typing octave up
        octaveShift++;
        return true;
      }
      else if (k.key() == '-')
      {
        // Piano roll zoom out
        shouldZoomOut = true;
        return true;
      }
      else if (k.key() == '=')
      {
        // Piano roll zoom in
        shouldZoomIn = true;
        return true;
      }
      else if (k.key() == ' ')
      {
        // Play/stop
        playhead = 0;
        playing = !playing;

        if (!playing)
        {
          playingNotes.clear();

          // Stop playing all notes when stopped
          int id = 0;

          std::vector<Pattern> *patterns = &song.patterns;

          // for (int i = 0; i < patterns->size(); i++)
          // {
          const Pattern *pattern = &patterns->at(currentPattern);

          for (int channel = 0; channel < pattern->channels.size(); channel++)
          {
            std::vector<Note> notes = pattern->channels.at(channel);

            for (int i = 0; i < notes.size(); i++)
            {
              triggerOff(-1 * (2 + id));
              id += 1;
            }
          }
          // }
        }
      }

      // Trigger synth note
      int midiNote = asciiToMIDI(k.key(), octaveShift * 12);

      if (midiNote > 0)
      {
        setParam("midiNote", midiNote);
        triggerOn(currentChannel, midiNote);
      }
    }

    return true;
  }

  bool onKeyUp(Keyboard const &k) override
  {
    if (!k.alt())
    {
      modifiers.erase("chord");
    }

    if (!k.shift())
    {
      modifiers.erase("zoom vertical");
    }

    if (k.key() == '`')
    {
      modifiers.erase("double");
    }

    if (k.key() == '1')
    {
      modifiers.erase("triplet");
    }

    if (k.key() == '-')
    {
      // End piano roll zoom out
      shouldZoomOut = false;
      return true;
    }
    else if (k.key() == '=')
    {
      // End piano roll zoom in
      shouldZoomIn = false;
      return true;
    }

    if (progress == progressSize)
    {
      int midiNote = asciiToMIDI(k.key(), octaveShift * 12);

      if (midiNote > 0)
      {
        triggerOff(midiNote);
      }
    }

    return true;
  }

  bool within(float time, float start, float distance)
  {
    return time >= start && time < start + distance;
  }

  void onAnimate(double dt) override
  {
    if (playing)
    {
      float playInterval = ((float)dt / 2.f) * (bpm / 120.f);
      int moveToNextPattern = 0;

      std::vector<Pattern> *patterns = &song.patterns;
      const Pattern *currentPatternData = &patterns->at(currentPattern);

      while (moveToNextPattern != -1)
      {
        int id = 0;
        const Pattern *pattern = &patterns->at(currentPattern);

        for (int channel = 0; channel < pattern->channels.size(); channel++)
        {
          std::vector<Note> notes = pattern->channels[channel];

          for (int i = 0; i < notes.size(); i++)
          {
            Note n = notes[i];

            if (n.id.empty())
            {
              song.patterns[currentPattern].channels[channel][i].id = generateUuid();
              n.id = song.patterns[currentPattern].channels[channel][i].id;
            }

            if (within(n.time, playhead, playInterval))
            {
              if (playingNotes.count(id) == 0)
              {
                synthManager.voice()->setInternalParameterValue("channel", channel);
                setParam("midiNote", n.pitch);
                triggerOn(channel, -1 * (2 + id));
                playingNotes.emplace(id);
              }
            }
            else if (within(n.time + n.duration, playhead, playInterval))
            {
              triggerOff(-1 * (2 + id));
              playingNotes.erase(id);
            }

            id++;
          }
        }

        if (playhead >= currentPatternData->length)
        {
          if (moveToNextPattern == 0)
          {
            moveToNextPattern = 1;
            playhead = 0;
            if (currentPattern != patterns->size() - 1)
            {
              currentPattern++;
            }
          }
        }
        else
        {
          moveToNextPattern = -1;
        }
      }

      playhead += playInterval;
    }
  }

  void onDraw(Graphics &g) override
  {
    navControl().active(!isImguiUsingInput());
    imguiBeginFrame();
    int winWidth = this->width();
    int winHeight = this->height();
    const int arrangeWidth = 89;
    const int inspectWidth = 197;
    const int mixerHeight = 268;
    float mixerPosition = mixerHeight;

    if (!mixerVisible)
      mixerPosition = 40;

    synthManager.voice()->setInternalParameterValue("channel", currentChannel);
    Channel *currentParams = &song.channels[currentChannel];
    float volume = currentParams->volume;
    int patch = currentParams->patch;

    std::vector<Pattern> *patterns = &song.patterns;
    Pattern *pattern = &patterns->at(currentPattern);
    std::vector<Note> *notes = &pattern->channels.at(currentChannel);

    if (aboutOpen)
    {
      if (addIntro(winWidth, winHeight, aboutProgress, 256))
      {
        aboutOpen = false;
        modalOpen = false;
      }

      if (aboutProgress < 512)
      {
        aboutProgress++;
      }
    }
    else
    {
      // Arrange Panel
      {
        windowColor(IM_COL32(43, 43, 43, 255));
        window("Arrange",
               ImVec2(0, 0),
               ImVec2(arrangeWidth, winHeight - 40));

        ImVec2 c = ImGui::GetCursorPos();
        ImVec2 cs = ImGui::GetCursorScreenPos();

        // Arrange header
        {
          ImGui::PushFont(fontHeading);

          drawRect(cs, IM_COL32(246, 141, 192, 255),
                   0, 0,
                   arrangeWidth, 24);

          xy(c, 8, 3);
          ImGui::Text("ARRANGE");

          ImGui::PopFont();
        }

        float actionHeight = 32;
        float arrangeActionsHeight = actionHeight * 2;

        // Arrange actions
        {
          ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7);

          ImGui::SetCursorPosX(4);
          ImGui::SetCursorPosY(ImGui::GetWindowHeight() - arrangeActionsHeight - 8);

          // Copy pattern
          if (ImGui::Button("COPY", ImVec2(arrangeWidth - 8, actionHeight)))
          {
            Pattern newPattern = Pattern();

            newPattern.name = pattern->name;
            newPattern.length = pattern->length;

            // Use different id for copy
            newPattern.id = generateUuid();

            for (int i = 0; i < pattern->channels.size(); i++)
            {
              std::vector<Note> newChannel = {};
              std::vector<Note> thisChannel = pattern->channels.at(i);

              for (int j = 0; j < thisChannel.size(); j++)
              {
                newChannel.push_back(thisChannel.at(j));
              }

              newPattern.channels.push_back(newChannel);
            }

            patterns->insert(patterns->begin() + currentPattern + 1, newPattern);
            currentPattern++;
            pattern = &patterns->at(currentPattern);
            playhead = 0;
          }

          ImGui::SetCursorPosX(4);

          // Delete pattern
          if (ImGui::Button("ERASE", ImVec2(arrangeWidth - 8, actionHeight)))
          {
            if (patterns->size() > 1)
            {
              patterns->erase(patterns->begin() + currentPattern);

              if (currentPattern != 0)
              {
                currentPattern--;
              }

              pattern = &patterns->at(currentPattern);
              playhead = 0;
            }
          }

          ImGui::PopStyleVar();
        }

        // Pattern list
        {
          xy(c, 0, 24);
          c = ImGui::GetCursorPos();
          ImGui::BeginChild("PatternList", ImVec2(arrangeWidth, ImGui::GetWindowHeight() - c.y - arrangeActionsHeight - 12), false);
          c = ImGui::GetCursorPos();
          xy(c, 8, 8);

          ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);

          int swapPatternsIndex = -1;

          // Render all patterns in project
          for (int i = 0; i < patterns->size(); i++)
          {
            if (addPatternItem(
                    i, patterns->at(i).id,
                    patterns->at(i).name,
                    i == currentPattern))
            {
              currentPattern = i;
              if (playing)
                playhead = 0;
            }

            if (ImGui::IsItemActive() && !ImGui::IsItemHovered() && swapPatternsIndex == -1)
            {
              int i_swap = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
              if (i_swap >= 0 && i_swap < patterns->size())
              {
                swapPatternsIndex = i_swap;
              }
            }
          }

          if (swapPatternsIndex != -1)
          {
            std::iter_swap(patterns->begin() + currentPattern, patterns->begin() + swapPatternsIndex);
            currentPattern = swapPatternsIndex;
            ImGui::ResetMouseDragDelta();
          }

          // Place new pattern button at bottom
          if (ImGui::Button("+", ImVec2(74, 46)))
          {
            // Add new pattern to project when clicked
            Pattern newPattern = Pattern();
            newPattern.name = "";
            newPattern.length = 4;
            newPattern.id = generateUuid();
            for (int i = 0; i < pattern->channels.size(); i++)
            {
              std::vector<Note> newChannel = {};
              newPattern.channels.push_back(newChannel);
            }
            patterns->push_back(newPattern);
            currentPattern = patterns->size() - 1;
            pattern = &patterns->at(currentPattern);
            if (playing)
              playhead = 0;
          }

          ImGui::PopStyleVar();
        }
      }

      // Inspect Panel
      {
        float attack = currentParams->attack;
        float cutoff = currentParams->lp;
        float release = currentParams->release;
        float tune = currentParams->tune;
        bool interpolate = currentParams->soft;

        {
          // UI
          {
            windowColor(IM_COL32(37, 37, 37, 255));
            window("Inspect",
                   ImVec2(arrangeWidth, 0),
                   ImVec2(inspectWidth + 1, winHeight - 40));

            ImVec2 c = ImGui::GetCursorPos();
            ImVec2 cs = ImGui::GetCursorScreenPos();

            // Pattern header
            {
              ImGui::PushFont(fontHeading);
              ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32_BLACK);

              drawRect(cs, IM_COL32_WHITE,
                       0, 0,
                       inspectWidth + 1, 24);

              xy(c, 8, 3);
              ImGui::Text("PATTERN / SONG");

              ImGui::PopStyleColor();
              ImGui::PopFont();
            }

            // Pattern params
            {
              // char* patternName = (char*)pattern->name.c_str();
              float patternLength = pattern->length;

              xy(c, 0, 24);
              addParameter(inspectWidth, "SIZE", &patternLength, 0.25f, 16.f, 0.25f, "float");

              addParameter(inspectWidth, "TEMPO", &bpm, 60.f, 240.f, 1.f);

              // Update selected pattern data to inspector values
              // pattern->name = std::string(patternName);
              pattern->length = patternLength;
            }

            ImGui::SetCursorPosX(0);
            float channelInspectorOffset = 24 + 31 + 1;

            // Channel header
            {
              ImGui::PushFont(fontHeading);
              ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32_BLACK);

              drawRect(cs, IM_COL32_WHITE,
                       0, channelInspectorOffset + 0,
                       inspectWidth + 1, 24);

              xy(c, 8, channelInspectorOffset + 3);
              ImGui::Text("CHANNEL");

              ImGui::PopStyleColor();
              ImGui::PopFont();
            }

            // Channel params
            {
              xy(c, 0, channelInspectorOffset + 24);
              addParameter(inspectWidth, "ATK.", &attack, 0.f, 6.f, 0.01f);
              addParameter(inspectWidth, "LP", &cutoff, 0.f, 20000.f, 200.f, "hz");
              addParameter(inspectWidth, "REL.", &release, 0.01f, 6.f, 0.01f);
              addParameter(inspectWidth, "TUNE", &tune, -12.f, 12.f, 0.1f);
              addParameter(inspectWidth, "VOL.", &volume, 0.f, 1.5f, 0.01f);
              addBoolParameter(inspectWidth, "SOFT", &interpolate);
            }

            // Patch browser
            {
              xy(c, 0, ImGui::GetCursorPos().y + 32);
              c = ImGui::GetCursorPos();

              ImGui::BeginChild("PatchBrowser", ImVec2(inspectWidth, ImGui::GetWindowHeight() - c.y), false);

              int patchNumber = 0;

              for (int i = 0; i < SOUND_BANK.size(); i++)
              {
                Patch* patchData = SOUND_BANK.at(i);

                if (patchData->id == -1)
                {
                  // is list separator, don't count for patch numbering
                  patchNumber--;
                }
                
                if (addPatchItem(
                        colorForChannel(currentChannel),
                        inspectWidth,
                        patchNumber, patchData->id, patchData->name,
                        patchData->id == patch))
                {
                  patch = patchData->id;
                }
                patchNumber++;
              }
            }
          }

          currentParams->attack = attack;
          currentParams->release = release;
          currentParams->tune = tune;
          currentParams->soft = interpolate;
          currentParams->patch = patch;
          currentParams->volume = volume;
          currentParams->lp = cutoff;
        }
      }

      // Save/Load Panel
      {
        windowColor(IM_COL32(246, 141, 192, 255));

        window("Save/Load",
               ImVec2(0, winHeight - 40),
               ImVec2(arrangeWidth + inspectWidth, 40));

        ImGui::SetCursorPos(ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7);
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.6f, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_WHITE);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(246, 141, 192, 255));

        if (ImGui::Button("LOAD", ImVec2(64, 32)))
        {
          std::ifstream is("song.json");
          cereal::JSONInputArchive archive(is);
          archive(song);

          if (song.channels.size() < NUM_CHANNELS)
          {
            std::cout << "Song has " << song.channels.size() << " channels, readjusting to " << NUM_CHANNELS << std::endl;

            for (int i = 0; i < song.patterns.size(); i++)
            {
              for (int j = song.channels.size(); j < NUM_CHANNELS; j++)
              {
                song.patterns[i].channels.push_back({});
              }
            }

            for (int j = song.channels.size(); j < NUM_CHANNELS; j++)
            {
              Channel channel = Channel();
              channel.patch = 0;
              channel.attack = 0.001f;
              channel.release = 0.f;
              channel.volume = 1.f;
              channel.pan = 0.f;
              channel.tune = 0.f;
              channel.lp = 20000.f;
              channel.soft = true;
              song.channels.push_back(channel);

              std::cout << "Created Channel " << j << std::endl;
            }
          }

          currentPattern = 0;
          pattern = &patterns->at(currentPattern);
          playhead = 0;
        }

        ImGui::SameLine();
        if (ImGui::Button("ABOUT", ImVec2(64, 32)))
        {
          aboutOpen = true;
          aboutProgress = 0;
          modalOpen = true;
        }

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 4 - 64);

        if (ImGui::Button("SAVE", ImVec2(64, 32)))
        {
          std::ofstream os("song.json", std::ios::binary);
          cereal::JSONOutputArchive archive(os);

          archive(song);
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
      }

      // Mixer Panel
      {
        static int selection = 0;

        PolySynth *synth = &synthManager.synth();
        SynthVoice *voice = synth->getActiveVoices();
        std::vector<float> left = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        std::vector<float> right = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        while (voice)
        {
          PCMEnv *pcmVoice = static_cast<PCMEnv *>(voice);
          int voiceChannel = pcmVoice->getInternalParameterValue("channel");
          left[voiceChannel] += pcmVoice->mFollow1.value() * 3;
          right[voiceChannel] += pcmVoice->mFollow2.value() * 3;
          voice = voice->next;
        }

        // UI
        {
          windowColor(IM_COL32(0, 0, 0, 0));

          window("Mixer",
                 ImVec2(arrangeWidth + inspectWidth + 1, winHeight - mixerPosition),
                 ImVec2(winWidth - (inspectWidth + 1), mixerHeight));

          ImVec2 c = ImGui::GetCursorScreenPos();

          // Background
          {
            drawRect(c, IM_COL32(44, 44, 44, 255),
                     ImGui::GetScrollX(), MIXER_TAB_HEIGHT,
                     ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 8);
          }

          // Mixer channels
          {
            ImGui::SetCursorPosX(0);
            ImGui::SetCursorPosY(8);

            for (int channel = 0; channel < NUM_CHANNELS; channel++)
            {
              float volume = song.channels.at(channel).volume;
              float pan = song.channels.at(channel).pan;

              addMixerChannel(
                  260, channel,
                  &selection, &volume, &pan,
                  colorForChannel(channel),
                  left[channel], right[channel]);

              song.channels[channel].volume = volume;
              song.channels[channel].pan = pan;
            }
          }
        }

        currentChannel = selection;
        synthManager.voice()->setInternalParameterValue("channel", currentChannel);
      }
    }

    if (!aboutOpen)
    {
      // Piano Roll
      {
        const float EDITOR_WINDOW_X = arrangeWidth + inspectWidth + 1;
        const float EDITOR_WINDOW_Y = 0;
        const float EDITOR_WINDOW_WIDTH = winWidth - (arrangeWidth + inspectWidth + 1);
        const float EDITOR_WINDOW_HEIGHT = winHeight - mixerPosition + MIXER_TAB_HEIGHT - 1;

        /* Temporary modifer for double division - e.g. 1/16 > 1/32 */
        float currentDivision = division;
        if (modifiers.count("double"))
          currentDivision *= 2.f;

        /* Temporary modifier for triplet - e.g. 1/16 > 1/12 and 1/32 > 1/24 */
        if (modifiers.count("triplet"))
          currentDivision *= 0.75f;

        /* Get screen space needed for whole pattern */
        const float contentWidth = getScreenWidth(baseWidth, timeToBaseX(pattern->length));
        const float contentHeight = getScreenHeight(baseHeight);

        /* Create window */
        windowColor(IM_COL32(32, 32, 32, 255));

        ImGui::SetNextWindowContentSize(
            ImVec2(contentWidth, contentHeight));

        window(
            "Piano Roll",
            ImVec2(EDITOR_WINDOW_X, EDITOR_WINDOW_Y),
            ImVec2(EDITOR_WINDOW_WIDTH, EDITOR_WINDOW_HEIGHT));
        ImVec2 cs = ImGui::GetCursorScreenPos();

        ImGui::PushFont(fontNumbers);

        /* Get scroll position */
        float scrollX = ImGui::GetScrollX();
        float scrollY = ImGui::GetScrollY();

        /* Scroll to center once content height defined */
        if (shouldScrollToCenter && contentHeight > 0)
        {
          ImGui::SetScrollY(
              contentHeight -
              (contentHeight / 2) - (EDITOR_WINDOW_HEIGHT / 2));

          shouldScrollToCenter = false;
        }

        /* Get cursor position local to window */
        ImVec2 cursor = ImGui::GetMousePos();
        cursor.x -= EDITOR_WINDOW_X;
        cursor.y -= EDITOR_WINDOW_Y;

        /* Get grid details at cursor */
        ImVec4 cursorBase = baseFloor(currentDivision, screenToBase(
                                                          ImVec2(cursor.x, cursor.y),
                                                          baseWidth, baseHeight,
                                                          ImGui::GetScrollX(), ImGui::GetScrollY()));

        ImVec4 cursorDiv = baseToDiv(currentDivision, cursorBase);
        float timeAtCursor = baseXToTime(cursorBase.x);
        float pitchAtCursor = cursorDiv.y;

        /* Mouse events */
        if (!modalOpen) // Only allow interacting with piano roll when modal isn't open
        {
          if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
          {
            if (
                (cursor.x < 0 || cursor.x > EDITOR_WINDOW_WIDTH) ||
                (cursor.y < 0 || cursor.y > EDITOR_WINDOW_HEIGHT))
            {
              // Don't allow mouse events outside of window to trigger editing
              justManipulatedNote = true;
            }
            else
            {
              if (!creatingNewNotes)
              {
                // Delete existing note if clicked
                for (int i = 0; i < notes->size(); i++)
                {
                  const Note existingNote = notes->at(i);

                  if (
                      pitchAtCursor == existingNote.pitch &&
                      timeAtCursor >= existingNote.time &&
                      // Correct for tiny floating point imprecision
                      timeAtCursor < existingNote.time + existingNote.duration - 0.001)
                  {
                    notes->erase(notes->begin() + i);
                    justManipulatedNote = true;
                    break;
                  }
                }

                if (!justManipulatedNote)
                {
                  justManipulatedNote = true;

                  // Only create note within range of pattern
                  if (timeAtCursor >= 0 && timeAtCursor < pattern->length)
                  {
                    creatingNewNotes = true;
                    addingNotes = true;
                    resizingNotes = false;

                    Note n = {
                        pitchAtCursor,
                        timeAtCursor,
                        divToTime(currentDivision, currentDivision),
                        generateUuid()};

                    newNotesStart = n;
                    newNotesMax = n;
                    newNotes.clear();
                    newNotes.push_back(n);
                    mouseMoveAmount = 0;

                    // Play note preview
                    flipTheFlop();
                    setParam("midiNote", pitchAtCursor);
                    triggerOn(currentChannel, (HIGHEST_NOTE * flipflop) + pitchAtCursor);
                    notePreviews.emplace(pitchAtCursor);
                  }
                }
              }
              else
              {
                if (addingNotes)
                {
                  if (!justManipulatedNote)
                  {
                    // Clicking creates note
                    if (timeAtCursor >= newNotesStart.time)
                    {
                      // Check if there exists note in the new notes that's already at this pitch
                      for (int i = 0; i < newNotes.size(); i++)
                      {
                        Note n = newNotes.at(i);

                        if (
                            n.pitch == pitchAtCursor)
                        {
                          // Delete the note
                          newNotes.erase(newNotes.begin() + i);
                          i -= 1;

                          // If on same cell, prevent new note from being created until next click
                          if (n.time == timeAtCursor)
                          {
                            justManipulatedNote = true;
                            noteAlreadyAtCursor = true;

                            // Play preview of erased note
                            setParam("midiNote", n.pitch);
                            triggerOn(currentChannel, (HIGHEST_NOTE * flipflop) + n.pitch);
                            notePreviews.emplace(n.pitch);
                          }

                          break;
                        }
                      }

                      if (!noteAlreadyAtCursor)
                      {
                        justManipulatedNote = true;

                        // Only create notes within range of pattern
                        if (timeAtCursor >= 0 && timeAtCursor < pattern->length)
                        {
                          mouseMoveAmount = 0;

                          Note n = {
                              pitchAtCursor,
                              timeAtCursor,
                              divToTime(currentDivision, currentDivision),
                              generateUuid()};

                          // Keep track of which note created is furthest to right
                          if (n.time >= newNotesMax.time)
                          {
                            newNotesMax = n;
                          }

                          // Restart already playing previews
                          for (float notePitch : notePreviews)
                          {
                            triggerOff((HIGHEST_NOTE * flipflop) + notePitch);
                          }

                          // Play "chord preview" of all notes created so far
                          flipTheFlop();
                          for (int i = 0; i < newNotes.size(); i++)
                          {
                            float notePitch = newNotes.at(i).pitch;

                            if (notePitch != n.pitch) // If same pitch occurs twice, only play preview once
                            {
                              setParam("midiNote", notePitch);
                              setParam("velocity", 0.75);
                              triggerOn(currentChannel, (HIGHEST_NOTE * flipflop) + notePitch);
                            }
                          }

                          // Play the new note at an emphasized volume
                          newNotes.push_back(n);
                          setParam("midiNote", n.pitch);
                          setParam("velocity", 1);
                          triggerOn(currentChannel, (HIGHEST_NOTE * flipflop) + n.pitch);
                          notePreviews.emplace(n.pitch);
                        }
                      }
                    }
                  }
                  else
                  {
                    // If mouse down, drag right to initiate note resize
                    if (!noteAlreadyAtCursor) // Only allow resizing to initate during a drag that adds a note, not removes
                    {
                      ImVec2 delta = ImGui::GetMouseDragDelta();

                      if (delta.x > 0)
                      {
                        // Dragged right, prepare for resize
                        mouseMoveAmount += 1;

                        if (mouseMoveAmount > 2)
                        {
                          justManipulatedNote = false;
                          addingNotes = false;
                          resizingNotes = true;
                        }
                      }
                    }
                  }
                }
                else if (resizingNotes)
                {
                  // Prevent resizing notes to negative values
                  if (timeAtCursor >= newNotesMax.time)
                  {
                    // Resize all created notes to extend up to the cursor
                    for (int i = 0; i < newNotes.size(); i++)
                    {
                      float clampedTimeAtCursor = timeAtCursor + divToTime(currentDivision, currentDivision);

                      // Only allow notes to extend to end of pattern
                      if (clampedTimeAtCursor > pattern->length)
                      {
                        clampedTimeAtCursor = pattern->length;
                      }

                      newNotes[i].duration = clampedTimeAtCursor - newNotes.at(i).time;
                    }
                  }
                }
              }
            }
          }
          else
          {
            if (creatingNewNotes)
            {
              if (addingNotes && modifiers.count("chord"))
              {
                // Allow more notes to be added by clicking while option held during note creation
                justManipulatedNote = false;
                noteAlreadyAtCursor = false;
                mouseMoveAmount = 0;
              }
              else
              {
                // Finish note creation
                addingNotes = false;
                resizingNotes = false;
                creatingNewNotes = false;
                mouseMoveAmount = 0;

                // Stop preview of all notes
                for (float notePitch : notePreviews)
                {
                  triggerOff((HIGHEST_NOTE * 2) + notePitch);
                  triggerOff((HIGHEST_NOTE * 1) + notePitch);
                }

                notePreviews.clear();

                // Transfer created notes to pattern
                for (int i = 0; i < newNotes.size(); i++)
                {
                  Note n = newNotes.at(i);

                  // Delete existing notes that new notes overlap
                  for (int j = 0; j < notes->size(); j++)
                  {
                    Note existingNote = notes->at(j);

                    // -0.001 corrects for tiny floating point imprecision
                    if (
                        n.pitch == existingNote.pitch &&
                        ((
                            n.time >= existingNote.time ||
                            n.time + n.duration - 0.001 > existingNote.time) &&
                        (n.time < existingNote.time + existingNote.duration - 0.001 ||
                          n.time + n.duration <= existingNote.time + existingNote.duration - 0.001)))
                    {
                      notes->erase(notes->begin() + j);
                      j--;
                    }
                  }

                  notes->push_back(n);
                }

                newNotes.clear();
              }
            }
            else
            {
              justManipulatedNote = false;
            }
          }
        }

        /* Draw grid */
        ImVec2 c = ImGui::GetCursorPos();

        for (float pitch = 0; pitch <= HIGHEST_NOTE; pitch++)
        {
          for (float div = 0; div < timeToDiv(currentDivision, pattern->length); div += currentDivision)
          {
            const ImVec4 cellBaseLoc = divToBase(currentDivision, div, pitch);
            const ImVec4 cellScreenLoc = baseToScreen(cellBaseLoc, baseWidth, baseHeight, CELL_GAP);

            float cellGapOffset = 0;

            // Determine cell color (normal, beat, measure)
            short cellColor = 48;

            // Check if cell lands on a 1/16, which always includes every beat and measure start
            float nextBase = cellBaseLoc.x + divToBaseX(currentDivision, currentDivision);

            if (fmodf(cellBaseLoc.x, 16) == 0)
            {
              // Color measure start
              cellColor += 24;
            }
            else if (fmodf(cellBaseLoc.x, 4) == 0)
            {
              // Color beat start
              cellColor += 8;
            }
            else if (
                fmodf(nextBase, 16) == 0 &&
                nextBase != timeToBaseX(pattern->length))
            {
              // Add gap before start of next measure
              cellGapOffset += 2;
            }

            // Color cell if whole tone in the twelve-tone scale
            float twelveTonePitch = (int)pitch % 12;

            if (
                twelveTonePitch != 1 &&
                twelveTonePitch != 3 &&
                twelveTonePitch != 6 &&
                twelveTonePitch != 8 &&
                twelveTonePitch != 10)
            {
              cellColor += 8;
            }

            // Color every octave start
            if (twelveTonePitch == 0)
            {
              cellColor += 24;
            }

            // Color middle C
            if (pitch == 60)
            {
              cellColor += 24;
            }

            // Color cells under playhead
            if (
                playing &&
                divToTime(currentDivision, div) <= playhead &&
                divToTime(currentDivision, div) + divToTime(currentDivision, currentDivision) > playhead)
            {
              cellColor += 24;
            }

            // Color cell and nearby cells when hovered
            if (
                div >= cursorDiv.x - currentDivision + (abs(pitch - pitchAtCursor) * currentDivision) &&
                div <= cursorDiv.x + currentDivision - (abs(pitch - pitchAtCursor) * currentDivision) &&
                pitch >= pitchAtCursor - 1 &&
                pitch <= pitchAtCursor + 1)
            {
              cellColor += 8;

              // Make directly hovered cell brightest
              if (div == cursorDiv.x && pitch == pitchAtCursor)
              {
                cellColor += 64;
              }
            }

            // Draw cell background
            drawRect(cs, IM_COL32(cellColor, cellColor, cellColor, 255),
                    cellScreenLoc.x,
                    cellScreenLoc.y,
                    cellScreenLoc.w - cellGapOffset,
                    cellScreenLoc.z);
          }

          // Draw drum sample labels if kit selected
          if (patch == 33)
          {
            Sample *sample = SOUND_BANK_MAP.at(patch)->getSample(pitch);

            if (sample != nullptr)
            {
              xy(
                  c,
                  ImGui::GetScrollX(),
                  baseToScreen(divToBase(currentDivision, 0, pitch), baseWidth, baseHeight, CELL_GAP).y);

              ImGui::Text("%s", sample->displayName.c_str());
            }
          }
        }

        /* Draw notes in other channels of pattern */
        for (int channel = 0; channel < NUM_CHANNELS; channel++)
        {
          if (channel == currentChannel)
            continue;

          ImU32 color = transparent(colorForChannel(channel), 0.25);

          if (allChannelsOpaque)
            color = transparent(colorForChannel(channel), 1);

          std::vector<Note> channelNotes = pattern->channels.at(channel);

          for (int i = 0; i < channelNotes.size(); i++)
          {
            Note n = channelNotes.at(i);
            const ImVec4 noteCoords = baseToScreen(noteToBase(n), baseWidth, baseHeight, CELL_GAP);

            drawRect(cs, color,
                    noteCoords.x,
                    noteCoords.y,
                    noteCoords.w,
                    noteCoords.z);
          }
        }

        /* Draw notes in current channel of pattern */
        for (int i = 0; i < notes->size(); i++)
        {
          Note n = notes->at(i);
          ImU32 color = colorForChannel(currentChannel);

          const ImVec4 noteCoords = baseToScreen(noteToBase(n), baseWidth, baseHeight, CELL_GAP);

          // Fade note if hovered
          if (
              pitchAtCursor == n.pitch &&
              timeAtCursor >= n.time &&
              // Correct for tiny floating point imprecision
              timeAtCursor < n.time + n.duration - 0.001)
          {
            color = transparent(color, 0.625);
          }

          drawRect(cs, color,
                  noteCoords.x,
                  noteCoords.y,
                  noteCoords.w,
                  noteCoords.z);

          float bottom = noteCoords.y + noteCoords.z;
          float right = noteCoords.x + noteCoords.w;

          drawTri(cs, IM_COL32_WHITE,
                  ImVec2(right, bottom),
                  ImVec2(right - (baseHeight / 2.f), bottom),
                  ImVec2(right, bottom - (baseHeight / 2.f)));

          // ImGui::SetCursorPosX(noteCoords.x);
          // ImGui::SetCursorPosY(noteCoords.y);
          // ImGui::Text("%f", n.time);
          // ImGui::SetCursorPosX(noteCoords.x);
          // ImGui::Text("%f", n.duration);
        }

        /* Draw notes currently being added */
        for (int i = 0; i < newNotes.size(); i++)
        {
          Note n = newNotes.at(i);

          const ImVec4 noteCoords = baseToScreen(noteToBase(n), baseWidth, baseHeight, CELL_GAP);

          drawRect(cs, transparent(IM_COL32_WHITE, 0.75),
                  noteCoords.x,
                  noteCoords.y,
                  noteCoords.w,
                  noteCoords.z);

          float bottom = noteCoords.y + noteCoords.z;
          float right = noteCoords.x + noteCoords.w;

          drawTri(cs, colorForChannel(currentChannel),
                  ImVec2(right, bottom),
                  ImVec2(right - (baseHeight / 2.f), bottom),
                  ImVec2(right, bottom - (baseHeight / 2.f)));
        }

        /* Keep editor scroll position centered while zooming */
        if (shouldZoomIn || shouldZoomOut)
        {
          // Get current position of centermost base
          ImVec4 baseAtWindowCenter = screenToBase(
              ImVec2(EDITOR_WINDOW_WIDTH / 2.f, EDITOR_WINDOW_HEIGHT / 2.f),
              baseWidth, baseHeight, scrollX, scrollY);

          // Modify base size to zoom
          if (shouldZoomIn)
          {
            if (baseHeight < 60)
            {
              baseHeight += 0.5f;
            }

            if (!modifiers.count("zoom vertical"))
            {
              if (baseWidth < 65)
              {
                baseWidth += 0.5f;
              }
            }
          }

          if (shouldZoomOut)
          {
            if (baseHeight > 7)
            {
              baseHeight -= 0.5f;
            }

            if (!modifiers.count("zoom vertical"))
            {
              if (baseWidth > 12)
              {
                baseWidth -= 0.5f;
              }
            }
          }

          // Find where the base is now located after zooming
          const ImVec4 newPositionOfBase = baseToScreen(baseAtWindowCenter, baseWidth, baseHeight, CELL_GAP, scrollX, scrollY);

          // Adjust scroll position to recenter the cell
          ImGui::SetScrollX(scrollX + (newPositionOfBase.x + 0.2f - (EDITOR_WINDOW_WIDTH / 2.f)));
          ImGui::SetScrollY(scrollY + (newPositionOfBase.y + 0.2f - (EDITOR_WINDOW_HEIGHT / 2.f)));
        }

        ImGui::PopFont();
      }
    }

    // Show demo window for GUI toolkit reference
    // windowColor(IM_COL32(20, 20, 20, 255));
    // ImGui::ShowDemoWindow();

    // Draw GUI
    imguiEndFrame();
    g.clear();
    synthManager.render(g);
    imguiDraw();
  }

  void onExit() override { imguiShutdown(); }
};

int main()
{
  KonpuStudio konpuu;
  konpuu.configureAudio(48000., 128, 2, 0);
  konpuu.title("konpu-STUDIO");
  konpuu.dimensions(0, 0, 1130, 722);
  konpuu.start();
  return 0;
}
