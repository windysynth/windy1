# Windy1 Synthesizer
Windy1 is a teensy 4.1 based MIDI host synthesizer designed to work with usb class complaint Wind controllers (e.g., EWI5000, EWIUSB,NuEVI, NuRAD, WARBL2, etc.)
The patches are editable with the Vyzex EWI4000S editor software or internally through the UI.

## Credits
This original project is [windysynth/windy1](https://github.com/windysynth/windy1).

## Building the Windy1
This repo is configured to build with PlatformIO.

### cloning and dependencies
Required library variants are included as Git submodules under `3rdparty/`:

- `3rdparty/Audio` (Audio fork, pulsewidth_offset branch behavior)
- `3rdparty/hexefx_audiolib_F32` (ws_mods behavior)
- `3rdparty/OpenAudio_ArduinoLibrary` (windy1-platformio branch)

To clone the repo with all required submodules:

```bash
git clone --recurse-submodules https://github.com/ivaylomil/windy1.git
```

If you already cloned without submodules, initialize them with:

```bash
git submodule update --init --recursive
```

PlatformIO is configured to use these copies via `lib_extra_dirs`.

Build command:

```bash
pio run
```

Default build environment:

- `windy1_teensy41` : I2S DAC output-only build (`ENABLE_LINE_IN=0`)

Line-in enabled build environment:

- `windy1_teensy41_linein` : I2S DAC + optional I2S ADC line-in build (`ENABLE_LINE_IN=1`)

Examples:

```bash
pio run -e windy1_teensy41
pio run -e windy1_teensy41_linein
```

Upload command:

```bash
pio run -t upload
```

### optional line-in feature switch
Line-in is optional in both hardware and software.

The repo already provides two PlatformIO environments instead of requiring manual flag edits:

- `windy1_teensy41` : output-only build (no ADC line-in hardware required)
- `windy1_teensy41_linein` : line-in enabled build (I2S ADC hardware required, e.g. PCM1808)

### hardware
Current baseline hardware is:

- Teensy 4.1
- I2S DAC for output (PCM5102, UDA1334A, or compatible standard I2S DAC)
- PSRAM chip on Teensy 4.1 (required for current delay settings)
- microSD card for patch/FX files
- OLED and encoders for local UI

Optional line-in hardware:

- I2S ADC (for example PCM1808), enabled only when `ENABLE_LINE_IN=1`

Current firmware no longer depends on SGTL5000 runtime support.

For full wiring and bring-up details, see `docs/WINDY1_HARDWARE_BUILD_GUIDE.md`.
