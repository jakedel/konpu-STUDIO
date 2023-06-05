<p align="center">
  <img width="642" src="https://github.com/jakedel/konpu-STUDIO/raw/main/res/logo.png" />
</p>

# KONPU-studio
**Music Production Studio for Allolib**

Designed and developed by Jake Delgado

UCSB Intro to Allolib S22 Project.

## Introduction

KONPU-studio is a cross-platform digital music production and sequencer tool built with Allolib.

My goal is to create a toolkit for musicians and non-programmers to create dynamic, interactive music and audio for applications on the Allosphere platform.

## Features

### User Interface

- Beautiful user interface implemented with [dear imgui](https://github.com/ocornut/imgui)

<img src="https://github.com/jakedel/konpu-STUDIO/raw/main/res/screenshot.png" />

- **Piano roll:** Visually compose music on a scrollable, zoomable grid

- **Patch browser:** Choose between 37 built-in timbres and a drum kit

- **31-channel mixer:** Adjust volume and stereo pan of individual sound channels, and visualize what's currently playing

- **Channel tweaks:** Customize sound parameters per each channel

- **Arrangement:** Break up a song into smaller sections, which can be reordered, copied, and erased

### Audio
- Load WAV sample files (48khz, mono/stereo)

- Play samples at different pitches

- Modify sounds with attack and release envelopes, detune, volume and lowpass filter

- **Multisampling:** Timbres contain multiple samples at different pitches, and the synth will choose closest sample for a given note

- **Linear interpolation:** When playing samples at slower speeds, new sample frames are created between existing ones to make sound "smoother"

## Build & Run

Tested on macOS on MacBook Pro 14" (2021, M1 Max) and MacBook Air (2018, Intel).

Should also work on Linux and possibly Windows.

**cmake needs to be installed and in PATH.**

### Build
```
./configure.sh
./run.sh
```

### Run
```
./run.sh
```

## Limitations

**These are pretty significant and are top priority to fix. Pull requests are appreciated!**

- **⚠️ No confirmation on save/load**
  - When save clicked, everything in song.json is overwritten.
  - When load clicked, the active project is overwritten.
  - **Make sure to back up a copy of `song.json` if you make something good!!**

- **⚠️ Can only save/load from** `song.json`
  - **If you want to create a new project,** save the current project, then rename the current `song.json` to something else so it isn't overwritten.
  - Then, **restart the app** to start a fresh project.

## Bugs

- Stopping will cause notes on certain instruments to hang

## Keyboard Shortcuts

`SPACE`: Play/stop

`ALT`: Chord mode (Hold while inserting notes, keep holding while dragging right)

`CTRL`: Hold and click a field to edit precise value

`ˋ`: Insert 1/32 notes (Precision)

`1`: Insert 1/12 notes (Triplets)

`ˋ1`: Insert 1/24 notes (Precision triplets)

`4`: Toggle mixer

`8`: Toggle presentation mode (All channels opaque in piano roll)

`\`: Octave up

`]`: Octave down

`+`: Zoom in

`-`: Zoom out

`SHIFT +`: Zoom in vertically

`SHIFT -`: Zoom out vertically
