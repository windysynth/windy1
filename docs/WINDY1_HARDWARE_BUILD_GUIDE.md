# Windy1 Hardware Build Guide

This guide summarizes the hardware expected by the current Windy1 firmware and what you need to assemble a working unit.

## 1.1 Physical Controls

Windy1 uses two rotary encoders and two push buttons on the front panel:

- Top encoder: moves through menu items
- Top encoder push: enters or confirms the selected menu item
- Bottom encoder: changes the selected value or setting
- Bottom encoder push: goes back to the parent menu

The status LED gives a quick visual indication of activity and breath response.

## 1. Target Hardware Architecture

Windy1 is built around a Teensy 4.1 with PCM5102 I2S DAC output as the baseline, plus:

- USB host MIDI input (for wind controllers)
- OLED + two rotary encoders for local UI
- SD card for patch files
- PSRAM for long stereo delay effect
- Optional DIN MIDI in/out and jack-detect behavior
- Optional I2S ADC line-in hardware (for example PCM1808)

## 2. Required Parts (Core BOM)

1. Teensy 4.1
2. I2S DAC module for stereo output (see section 2a for options)
3. PSRAM on Teensy 4.1 underside pads (required by current delay setup)
4. microSD card (for patch and FX storage)
5. USB host wiring from Teensy host pads to USB-A connector
6. 2x rotary encoders with integrated push button
7. 1x SSD1306 128x64 I2C OLED
8. Power supply suitable for Teensy + audio path + USB host peripheral current

### 2a. I2S DAC Module Options

The firmware supports any standard I2S DAC (no codec control required). Two proven alternatives:

**PCM5102 (Recommended Baseline)**

- Widely available, low cost
- I2S input, stereo analog outputs
- 3.3V–5V operation
- Pinout: BCLK, LRCK, DIN/DATA, GND, power
- No firmware changes needed
- Teensy wiring used by this firmware:
  - Teensy pin 21 -> DAC BCLK
  - Teensy pin 20 -> DAC LRCK / LRC / WS
  - Teensy pin 7 -> DAC DIN / DATA
  - Teensy GND -> DAC GND
  - Teensy 3.3V or 5V -> DAC power, depending on module requirements
- MCLK is not used by the current Windy1 baseline with PCM5102 modules

**UDA1334A (Drop-in Replacement)**

- Texas Instruments stereo I2S DAC
- Pin-compatible I2S interface with PCM5102
- 2.7V–3.6V operation (verify datasheet for your module variant)
- Additional control pins (SF0, SF1, MUTE, DEEM, PLL) default to I2S mode
- Same Teensy wiring as PCM5102: pin 21 (BCLK), pin 20 (LRCK), pin 7 (DIN / DATA), GND, 3.3V
- Some UDA1334 breakout variants also require MCLK: Teensy pin 23 -> DAC SCLK / MCLK
- Do not leave control pins floating on breadboard builds; tie SF0/SF1/MUTE/DEEM/PLL to valid logic levels per module revision
- **No firmware code changes required** — standard I2S protocol

Both DACs support audio clocks at 12.288 MHz and 24.576 MHz (Teensy standard).

## 3. Optional / Feature-Complete Parts

1. 5-pin DIN MIDI IN/OUT circuits (UART + opto/protection as needed)
2. I2S ADC for line-in input (for example PCM1808)
3. Line-out jack detect circuitry to support mono fallback behavior
4. Custom PCB for Teensy 4.1 + external I2S DAC/ADC topology

## 3.1 Alternative Build Methods: Three-Layer Perfboard Stack

If you are migrating from breadboard to a more permanent assembly, see [docs/WINDY1_PERFBOARD_BUILD.md](WINDY1_PERFBOARD_BUILD.md) for a dedicated guide to building Windy1 on a stack of three 24×10 perfboards:

- **Layer 1:** Teensy 4.1 core and power distribution
- **Layer 2:** UDA1334 audio DAC and output routing
- **Layer 3:** UI hub for OLED display, encoders, and button/LED wiring

The perfboard guide includes layer layout strategy, complete interlayer wiring tables with exact pin assignments, mechanical assembly steps, staged bring-up validation, and perfboard-specific troubleshooting. It references this baseline hardware guide for shared fundamentals.

## 4. Firmware Pin Map

The current firmware uses these pins directly.

### UI and Controls

- Bottom encoder A: 36
- Bottom encoder B: 37
- Bottom encoder push: 32
- Top encoder A: 31
- Top encoder B: 30
- Top encoder push: 29
- Status LED: 13 (onboard LED)

### Power and Jack Sense

- Line-out right plug detect: 33
- Line-out left plug detect: 5
- Jack detect logic is active-high in firmware: a plugged jack should drive the sense pin high
- Mono fallback behavior is enabled when right detect is high and left detect is low

Pin 25 is no longer used for SGTL5000-style power-down handling.

### Audio I/O

- I2S DAC output wiring used by the current firmware:
  - Teensy pin 21 -> DAC BCLK
  - Teensy pin 20 -> DAC LRCK / LRC / WS
  - Teensy pin 7 -> DAC DIN / DATA
  - Teensy GND -> DAC GND
  - Teensy 3.3V or 5V -> DAC power as required by the specific module
- PCM5102 modules typically do not require MCLK
- Some UDA1334 modules require MCLK: Teensy pin 23 -> DAC SCLK / MCLK
- Optional I2S ADC line-in wiring when `ENABLE_LINE_IN=1`:
  - Teensy pin 21 -> ADC BCLK
  - Teensy pin 20 -> ADC LRCK / LRC / WS
  - Teensy pin 8 <- ADC DOUT / DATA
  - Teensy GND -> ADC GND
  - Teensy 3.3V or module-appropriate supply -> ADC power
- The optional ADC shares BCLK and LRCK with the DAC and adds only the ADC data line on pin 8

#### PCM1808 Line-In Wiring

For a PCM1808 breakout used as the optional ADC input path:

- PCM1808 OUT -> Teensy pin 8
- PCM1808 BCK -> Teensy pin 21
- PCM1808 LRC -> Teensy pin 20
- PCM1808 SCK -> Teensy pin 23 (required on modules that need master clock)
- PCM1808 GND -> Teensy GND
- PCM1808 LIN/RIN -> analog input source L/R

Recommended mode straps on PCM1808 breakout boards:

- `MD1 = LOW`
- `MD0 = LOW`  (slave mode)
- `FMY = LOW`  (I2S format)

Notes:

- Follow your module power requirements; many PCM1808 boards need both 5V analog and 3.3V digital domains (some breakouts regulate this internally).

### Display

- SSD1306 128x64 via second hardware I2C bus (Wire1)
- **Wiring:**
  - Display SDA → Teensy pin 17 (Wire1 SDA)
  - Display SCL → Teensy pin 16 (Wire1 SCL)
  - Display GND → Teensy GND
  - Display VCC → Teensy 3.3V
- I2C address: 0x3C (default) or 0x3D (if address pin tied high)
- No reset pin required (firmware pins it as unused)
- U8G2 library, 2nd hardware I2C mode

### MIDI

- DIN MIDI path: Serial1
- USB host MIDI: Teensy host controller pads + USBHost_t36 stack
- USB device MIDI is also active through Teensy USB configuration

## 5. Audio and DSP Assumptions

1. I2S DAC output path (PCM5102 or UDA1334A or compatible) is the default output hardware path.
2. Output level control is software gain in the audio graph (not codec hardware volume).
3. Delay object is instantiated with PSRAM usage enabled.
4. If PSRAM is missing, delay behavior may fail or degrade depending on branch/config.
5. Line-in is optional and controlled by software build flag plus hardware population.
6. SGTL5000 runtime initialization and shutdown behavior are not part of the current firmware baseline.

## 6. Storage Assumptions

- SD is initialized using BUILTIN_SDCARD.
- Firmware loads/creates patch files named 000.PAT..127.PAT.
- Global FX file is 000.FX.

## 7. Practical Build Levels

### Minimum Bring-Up (recommended first)

1. Teensy 4.1
2. I2S DAC output hardware (PCM5102, UDA1334A, or compatible)
3. PSRAM installed
4. SD card installed
5. One encoder + OLED + USB host input

### Full Feature Build

1. All minimum items
2. Two encoders and both push buttons
3. DIN MIDI IN/OUT hardware
4. Optional PCM1808 (or equivalent) for line-in path
5. Jack detect hardware for pins 33 and 5

## 8. Bring-Up Checklist

1. Confirm firmware builds and uploads to Teensy 4.1.
2. Verify OLED powers and menu renders.
3. Verify both encoders and buttons navigate/edit values.
4. Verify SD card mount and patch files are created/read.
5. Verify USB host MIDI receives note/CC from wind controller.
6. Verify audio output path from I2S DAC (PCM5102/UDA1334A/alternative) L/R outputs.
7. If line-in hardware is installed and enabled, verify Aux In control and input mix behavior.

## 9. Optional Line-In Configuration

Line-in is optional in both hardware and software.

1. Populate ADC hardware (for example PCM1808) only if you want line-in.
2. Build with line-in enabled when ADC hardware is present.
3. Build with line-in disabled when no ADC hardware is present; this disables Aux In behavior and keeps line-in mixer channels off.

Recommended compile-time flag:

- ENABLE_LINE_IN=0 : output-only build (no line-in hardware required)
- ENABLE_LINE_IN=1 : line-in enabled build (ADC hardware required)

Current PlatformIO environments:

- `windy1_teensy41` : output-only build
- `windy1_teensy41_linein` : line-in enabled build

## 10. Known Dependency Coupling to Hardware

Hardware functionality relies on the project using these library variants:

1. windysynth Audio fork (pulsewidth_offset branch behavior)
2. windysynth hexefx_audiolib_F32 (ws_mods behavior)
3. OpenAudio_ArduinoLibrary for F32 blocks/converters

If library variants are changed, revalidate power, waveform, and effect paths on hardware.

## 11. No Sound Debug Checklist (UDA1334 / PCM5102)

If the unit powers up but you get no audio from the DAC output, use this sequence.

### 11a. Confirm DAC wiring first

For UDA1334A, use the same I2S wiring as the baseline DAC path:

- Teensy pin 21 -> DAC BCLK
- Teensy pin 20 -> DAC LRCK / WS
- Teensy pin 7 -> DAC DIN / DATA
- Teensy GND -> DAC GND
- Teensy 3.3V -> DAC VCC (recommended for UDA1334 modules)

For UDA1334 breakout variants that need a master clock, also wire:

- Teensy pin 23 -> DAC SCLK / MCLK

Also verify module strap/control pins are in valid I2S playback state for your breakout revision (for example MUTE/de-emphasis/format pins if exposed). On breadboard prototypes, do not leave these pins floating.

Common starting point for many UDA1334 breakout modules: `MUTE=LOW`, `DEEM=LOW`, `SF0=LOW`, `SF1=LOW`, `PLL=LOW`.

### 11b. Verify I2S clocks on hardware

Use an oscilloscope or logic analyzer on the DAC pins:

- BCLK should be active (typically around a few MHz)
- LRCK should be active (audio sample rate, typically about 44.1 kHz)
- DIN should toggle when notes are playing

Interpretation:

- No BCLK/LRCK: wiring, pin mapping, or startup issue before audio data stage
- BCLK/LRCK present but DIN flat: synth path is likely muted/gated in firmware
- All three active but still no analog output: DAC module strap/power/analog output stage issue

### 11c. Firmware behaviors that can look like DAC failure

Windy1 can be silent even with correct DAC wiring because of control state:

1. Master volume may restore as zero from EEPROM.
2. Breath-controlled amplitude path may remain near zero without the expected breath CC.

Current default behavior:

- `breath_cc` defaults to CC2 (Breath) unless changed in menu/EEPROM.
- If your controller sends expression on a different CC (for example CC11), set `BreathCC` in the UI to match.
- Note On velocity response is controlled by the `VelMode` System setting:
  - `Default` (0): velocity seeds breath amplitude at note attack when breath CC is zero (original behavior)
  - `BrOnly` (1): velocity never affects amplitude; note is silent until breath CC arrives — recommended for wind controllers that send breath before Note On
  - `Hybrid` (2): velocity is used only when it exceeds the current breath level, giving harder tongued attacks more presence
  - Setting persists across power cycles (EEPROM-backed, found in `System...` → `VelMode:`)

### 11d. Use debug build to confirm live MIDI/control data

Use the debug environment to print control activity over USB serial:

- Build/upload environment: `windy1_teensy41_debug`
- This enables `DEBUG_USB_SERIAL` and routes debug prints to USB serial
- MIDI input tracing is enabled with `DEBUG_MIDI_INPUT` in this debug environment

While testing, confirm you see:

1. Note On messages
2. Matching breath/control CC messages for the configured `BreathCC`

Typical debug lines now include:

- Per-source summary (rate-limited):
  - `[MIDI HOST] on:12 off:12 cc:148 pc:0 pb:0 nrpn:0 mm:0 bcc:2`
- CC mismatch warning (rate-limited):
  - `[MIDI DEV] CC mismatch got:11 expected:2 val:90`
- NRPN updates:
  - `[MIDI NRPN] M:64 L:5 D:87`

If Note On arrives but breath/control does not, audio can remain effectively muted.

### 11e. Fast isolation test

To separate DAC hardware issues from control-path gating:

1. Send Note On with non-zero velocity.
2. Send CC2 (or set `BreathCC` to the CC your controller actually sends).
3. Raise master volume in UI.

If audio appears only after step 2 or 3, DAC wiring is likely fine and the issue is control mapping/state.

### 11f. Legato Assist for controllers with NoteOff gaps

Some wind controllers send brief NoteOff gaps between finger transitions. That can retrigger attacks even when breath is continuous.

Windy1 now includes optional global System settings for this case:

- `LegatoA` (Off/On): enables deferred note release handling
- `LegHold` (ms): hold window used by timeout-based policies
- `BreathTh` (0..127): breath threshold used to enter/exit deferred release
- `LegPol`: release policy selector

Release policies:

- `BreathOnl`: release only when breath falls below the internal release threshold
- `BrOrTime`: release when breath drops below threshold or hold time expires
- `TimeOnly`: release only when hold time expires

Behavior and defaults:

- Default is compatibility-safe: `LegatoA=Off`
- With `LegatoA=Off`, release/retrigger behavior follows the existing control path
- Thresholding uses internal hysteresis derived from `BreathTh` to reduce chatter near the threshold

Recommended starting points:

1. Keep `LegatoA=Off` for controllers that already provide smooth legato transitions.
2. For controllers with audible retrigger between slurred notes, start with:

- `LegatoA=On`
- `LegPol=BreathOnl`
- `BreathTh=10`
- `LegHold=40`

1. If notes hang too long, try `LegPol=BrOrTime` and reduce `LegHold`.

Debug validation tip:

- Use `windy1_teensy41_debug` and verify NoteOn/NoteOff/CC behavior while toggling `LegatoA`.
