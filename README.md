## Audio Synth with Visualization

This project is a simple software synthesizer with a console visualization.
It can generate polyphonic tracks, apply envelopes, and display real-time bar visualization of the sound.

Two backends are supported:

PortAudio → cross-platform (Linux, macOS, Windows).

ALSA → Linux only, using the native ALSA API.

## Features

Generate sound from scheduled tracks/notes.

Envelope + instrument shaping for realistic tones.

Stereo audio output.

Console bar visualization of the audio signal in real time.

Configurable buffer size, sample rate, and bar count.

## Requirements
Common

gcc or clang (C99).

make.

PortAudio version

PortAudio