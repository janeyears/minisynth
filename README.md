## Audio Synth with Visualization

This project is a simple software synthesizer with a console visualization.
It can generate polyphonic tracks, apply envelopes, and display real-time bar visualization of the sound.

## Build Instructions
### Requirements

PortAudio – cross-platform, used in portaudio_minisynth. Already included in ./portaudio_minisynth/libs/.

ALSA – Linux only, using the native ALSA API. Make sure ALSA development headers are installed (libasound2-dev on Debian/Ubuntu).

    C compiler (e.g., gcc or clang)

    Make

There are two separate builds:

ALSA version
    
    make alsa

PortAudio version
  
    make  portaudio

### Executables

minisynth_alsa – uses ALSA for audio output

minisynth_portaudio – uses PortAudio for audio output

By default, executables are moved to the root project directory for convenience.

### USAGE

    ./minisynth_alsa ./music/file.synth

or

    ./minisynth_portaudio ./music/file.synth

## Features

NOTE! The focus of this project is not error handling. So program will work only with valid input files.

* **Read from `.synth` files** using keywords like:

  ```text
  volumes <V>[,<V>,...]
  ```

* **Music Description Format:**
  Your minisynth program accepts a text-based file format (examples use the `.synth` extension, though any extension is allowed).

  * Empty lines or lines starting with `#` are ignored as comments.
  * The first non-comment line must specify the tempo:

    ```
    tempo <N>
    ```

    where `<N>` is an integer representing the tempo in beats per minute.
  * The next non-comment line must define the tracks:

    ```
    tracks <instrument>[,<instrument>,...]
    ```

    Each entry represents a track, numbered from 1 to the total number of tracks.

* **Track Notes:**

  * Each remaining non-comment line has the format:

    ```
    <track>:<notes>
    ```

    where `<track>` is the track number, and `<notes>` is a sequence of notes for that track.

  * **Note format:**

    ```
    <pitch>[<alteration>][<octave>][/<duration>]
    ```

    * `<pitch>`: a–g for notes, or `r` for a rest.
    * `<alteration>` (optional): `#` for sharp, `b` for flat.
    * `<octave>` (optional): 0–9, using standard scientific pitch notation (middle C is `c4`).
    * `<duration>` (optional): preceded by `/`, decimal or fractional number representing beats.

  * Notes inherit the previous note’s octave and duration if not explicitly specified. The first note of each line defaults to octave 4 and lasts 1 beat.

  * Notes are appended sequentially; a note begins immediately after the previous one ends. Silent notes can be used for spacing.

  * Other characters on the line are ignored (e.g., `|` for visual bar separators or spaces for readability).

* **Sound Generation:**

  * Generates audio from scheduled tracks and notes.
  * Supports envelope and instrument shaping for realistic tones.
  * Outputs **stereo audio**.
  * Configurable buffer size, sample rate, and bar count.

* **Default Audio Format:**

By default, the synthesizer generates audio as stereo, 16-bit signed integers at a sample rate of 44,100 Hz. This format is compatible with most sound cards, so no additional conversion is needed for playback.

## Visualizer

The synthesizer includes a real-time console bar visualizer that displays the magnitude of the audio signal per bar.

<img width="1121" height="460" alt="image" src="https://github.com/user-attachments/assets/354d94f3-3c63-4a37-9e35-201689ad3b8d" />

The visualizer updates based on the audio being generated, showing recent sound levels rather than cumulative values from the start of the song.

The number of bars, the update frequency, and the scaling can be configured in the program.

The visualizer works both with PortAudio and ALSA backends.
