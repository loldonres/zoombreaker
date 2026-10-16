# ZoomBreaker

> A Windows toolkit for Zoom meetings: soundpad, virtual camera bridge, chat spam, hand-raise control, participant parser, and audio effects — all in one dark-themed utility.

ZoomBreaker is a Win32 C++ application that integrates with Zoom via UI Automation, WinHTTP, and virtual audio/video devices. It does not modify Zoom's binary. Instead, it controls an already running Zoom client through keyboard emulation, UIA element inspection, and HTTP calls to a local virtual camera service.

## Features

- **Soundpad** — play WAV, MP3, FLAC, OGG, M4A files into headphones, a virtual cable (microphone), or both. Bass boost and volume control up to 1000%.
- **Virtual camera** — control a Neon-based virtual camera service via HTTP. Inject any video file into your Zoom camera feed.
- **Chat spam** — automated message flooding with configurable interval and limit.
- **Hand raise** — continuous Alt+Y hold for persistent raise-hand state.
- **Participant parser** — reads the visible participant list from the Zoom "Participants" window via UI Automation. Click any name to copy it to clipboard.
- **Auto Raid** — combined mode for automated microphone, camera, soundpad, and rename cycles.
- **Settings dialog** — adjustable volume slider range (0–1000%).

## Requirements

- Windows 10 or 11 (x64) 
- Zoom desktop client (Russian or English UI)
- [VB-Audio Virtual Cable](https://vb-audio.com/Cable/) for routing audio into the microphone
- [Neon](https://github.com/newdee/neon) virtual camera service for video injection

## Quick Building

ZoomBreaker is built with MinGW-w64 (GCC 13 or newer). All dependencies are vendored or linked statically.

To build:
- Open file build_mingw.bat

### Wanna edit it?
Add your dependencies in this *.bat* file.
