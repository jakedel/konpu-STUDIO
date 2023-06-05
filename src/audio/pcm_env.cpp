#include <iostream>
#include <map>
#include <cstdio>
#include <vector>

#ifndef _PCM_ENV_CPP
#define _PCM_ENV_CPP

// GAMMA
#include "Gamma/Analysis.h"
#include "Gamma/Effects.h"
#include "Gamma/Envelope.h"

// ALLOLIB AUDIO
#include "al/scene/al_PolySynth.hpp"
#include "al/sound/al_SoundFile.hpp"
#include "al/ui/al_Parameter.hpp"
#include "al/graphics/al_Shapes.hpp"

using namespace al;

const int NUM_CHANNELS = 32;

struct Sample
{
  int pitch_root = 60;
  int pitch_highest = 127;
  int sample_rate = 44100;
  std::string name;
  std::string displayName;
  std::vector<float> sampleData;

  Sample(std::string filename, int pitch_root, int pitch_highest, std::string displayName="")
  {
    this->name = filename;
    this->displayName = displayName;
    this->pitch_root = pitch_root;
    this->pitch_highest = pitch_highest;

    // load sample
    SoundFile file;
    file.open(filename.c_str());
    sample_rate = file.sampleRate;
    
    for (long long int i = 0; i < file.frameCount; i++)
    {
      sampleData.push_back(file.getFrame(i)[0]);
    }
  }
};

struct Patch
{
  std::string name;
  std::string lastLoadedSample = "";
  std::vector<Sample*> samples;
  int i_load = 0;
  int i_size = 0;
  int id = 0;
  Patch() {}
  virtual Sample* getSample(int index) { return nullptr; }
  virtual bool load() { return true; }
};

struct PatchListSeparator : Patch
{
  PatchListSeparator(std::string name)
  {
    this->id = -1;
    this->name = name;
  }
};

struct DrumKit : Patch
{
  std::map<std::string, int> sampleIndex;

  DrumKit(int id, std::string folder, std::vector<std::string> filenames, std::map<std::string, int> rootPitches)
  {
    this->id = id;
    this->name = folder;

    for (int i = 0; i < filenames.size(); i++)
    {
      int pitchAdjust = 0;

      if (rootPitches.count(filenames[i]))
      {
        pitchAdjust = rootPitches[filenames[i]];
      }

      samples.push_back(
        new Sample(
          "timbre/" + folder + "/" + filenames[i] + ".wav",
          0 - pitchAdjust,
          0,
          filenames[i]
        )
      );

      sampleIndex[filenames[i]] = i;
    }
  }

  Sample* getSample(int pitch)
  {
    int adjustedIndex = pitch - 60; // start with samples[0] at MIDI note 60

    if (adjustedIndex >= 0 && adjustedIndex < samples.size())
    {
      return samples[adjustedIndex];
    }
    else
    {
      return nullptr;
    }
  }

  bool load()
  {
    return true;
  }

  int s(std::string sampleName) {
    return sampleIndex[sampleName];
  }
};

struct Timbre : Patch
{
  std::string folder;
  std::vector<int> pitches;
  int correct = 0;
  
  Timbre(int id, std::string folder, std::string name, std::vector<int> pitches, int correct=0)
  {
    this->id = id;
    this->i_size = pitches.size();
    this->folder = folder;
    this->name = name;
    this->pitches = pitches;
    this->correct = correct;
  }

  bool load()
  {
    if (i_load < i_size)
    {
      int pitch = pitches[i_load];
      int nextPitch;

      if (i_load < pitches.size() - 1)
      {
        nextPitch = pitches[i_load + 1];
      }
      else
      {
        nextPitch = 127;
      }

      std::string samplePath = folder + "/" + name + "/" + std::to_string(pitch);

      samples.push_back(
        new Sample(
          "timbre/" + samplePath + ".wav",
          pitch - correct,
          nextPitch - 1 - correct
        )
      );

      this->lastLoadedSample = samplePath;
      i_load++;

      if (i_load == i_size) return true;
      return false;
    }

    return true;
  }

  Sample* getSample(int pitch)
  {
    // Find the sample that's most optimal for the pitch
    for (int i = 0; i < samples.size(); i++)
    {
      if (pitch <= samples[i]->pitch_highest)
      {
        return samples[i];
      }
    }

    return samples[0]; // Fallback to single sample
  }
};











class PCMEnv : public SynthVoice
{
public:
  gam::Pan<> mPan;
  gam::Sine<> mOsc;
  gam::Env<3> mAmpEnv;
  gam::EnvFollow<> mFollow1;
  gam::EnvFollow<> mFollow2;
  gam::OnePole<> mFilter;

  static std::map<int, Patch*>* SoundBankMap;
  static float Cutoff;
  float attenuation = 1;
  float rate = 0;
  float position = 0;
  Sample* sampleRef = nullptr;;
  Patch* currentPatch = nullptr;
  int sampleLength = 0;

  void init() override
  {
    // Intialize envelope
    mAmpEnv.curve(0); // make segments lines
    mAmpEnv.levels(0, 1, 1, 0);
    mAmpEnv.sustainPoint(2);

    // Set up lowpass filter
    mFilter.type(gam::LOW_PASS);

    createInternalTriggerParameter("channel", 0, 0, NUM_CHANNELS - 1);
    createInternalTriggerParameter("midiNote", 0, 0, 127);
    createInternalTriggerParameter("velocity", 1, 0, 20.0);

    createInternalTriggerParameter("patch", 37, 0, 255);
    createInternalTriggerParameter("amplitude", 1, 0.0, 20.0);
    createInternalTriggerParameter("attackTime", 0.001, 0.001, 3.0);
    createInternalTriggerParameter("releaseTime", 0.13, 0.001, 10.0);
    createInternalTriggerParameter("pan", 0.0, -1.0, 1.0);
    createInternalTriggerParameter("interpolate", 1, 0, 1);
    createInternalTriggerParameter("tune", 0, -12, 12);
    createInternalTriggerParameter("lp", 20000, 0, 20000);
  }

  float linear_interpolate(std::vector<float>& data, float position, int length) {
    int floored_position = floor(position);
    float current_item = data[floored_position];
    int next_position = floored_position + 1;

    if (next_position < length)
    {
      float next_item = data[next_position];
      float fraction = position - floored_position;

      return current_item + fraction * (next_item - current_item);
    }

    return current_item;
  }

  void onProcess(AudioIOData &io) override
  {
    bool interpolate = getInternalParameterValue("interpolate");
    mAmpEnv.lengths()[0] = getInternalParameterValue("attackTime");
    mAmpEnv.lengths()[2] = getInternalParameterValue("releaseTime");
    float cutoff = getInternalParameterValue("lp");
    mPan.pos(getInternalParameterValue("pan"));
    mFilter.freq(PCMEnv::Cutoff * (cutoff / 20000.f));
    
    while (io())
    {
      float s1 = 0;
      float s2;

      // Patch
      if (position >= sampleLength) {
        rate = 0;
      }

      float amp = getInternalParameterValue("amplitude");
      float vel = getInternalParameterValue("velocity");

      if (rate > 0) {
        if (interpolate) {
          s1 = linear_interpolate(sampleRef->sampleData, position, sampleLength);
        } else {
          s1 = sampleRef->sampleData.at(position);
        }
          
        s1 = s1 / attenuation;
        s1 = s1 * mAmpEnv() * amp * vel;
      }

      position += rate;

      // Filter
      if (PCMEnv::Cutoff < 20000 || cutoff < 20000) s1 = mFilter(s1);

      // Pan
      mPan(s1, s1, s2);

      // Sync visuals
      mFollow1(s1);
      mFollow2(s2);

      // Output
      io.out(0) = s1;
      io.out(1) = s2;

      if (position >= sampleLength || mAmpEnv.done()) {
        free();
      }
    }
  }

  void set(int timbre, float midiNote, float amplitude, bool interpolate, float releaseTime, float velocity, float cutoff)
  {
    setInternalParameterValue("patch", timbre);
    setInternalParameterValue("midiNote", midiNote);
    setInternalParameterValue("amplitude", amplitude);
    setInternalParameterValue("interpolate", interpolate);
    setInternalParameterValue("releaseTime", releaseTime);
    setInternalParameterValue("velocity", velocity);
    setInternalParameterValue("lp", cutoff);
  }

  void onTriggerOn() override {
    float midiNote = getInternalParameterValue("midiNote");
    float tune = getInternalParameterValue("tune");

    // Update currentPatch with timbre parameter
    this->currentPatch = PCMEnv::SoundBankMap->at(getInternalParameterValue("patch"));

    // Use midiNote to find the best sample in timbre
    this->sampleRef = this->currentPatch->getSample(
      static_cast<int>(floor(midiNote)) // Determine multisample based on MIDI note as int
    );

    // Only play sample if exists
    if (this->sampleRef)
    {
      if (getInternalParameterValue("patch") == 33)
      {
        midiNote = 0; // Don't use index for pitch on drum kit
      }

      // Convert midiNote to frequency to set playback rate
      this->rate = pow(2.f, (midiNote + tune - this->sampleRef->pitch_root) / 12.f);
      
      // Prepare playback of sample
      this->sampleLength = this->sampleRef->sampleData.size();
      this->attenuation = 2.75;
      this->position = 0;

      // Reset envelope
      mAmpEnv.reset();
    }
    else
    {
      this->rate = 0;
    }
  }

  void onTriggerOff() override {
    mAmpEnv.release();
  }
};

float PCMEnv::Cutoff = 20000.f;
std::map<int, Patch*>* PCMEnv::SoundBankMap = nullptr;

#endif
