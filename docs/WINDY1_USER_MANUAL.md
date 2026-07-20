# Windy1 User Manual

This manual is for day-to-day use of Windy1: getting sound quickly, navigating menus, configuring controllers, and troubleshooting common performance issues.

For wiring, BOM, and electrical bring-up details, see docs/WINDY1_HARDWARE_BUILD_GUIDE.md.

## 1. Quick Start (First Sound)

Use this checklist for a first successful boot:

1. Power Teensy 4.1 and connect your DAC output (PCM5102/UDA1334) to speakers or headphones amp.
2. Connect your wind controller to Windy1 USB host input.
3. Confirm OLED menu is visible.
4. Raise `Volume` in top menu.
5. Open `System...` and verify:
   - `BreathCC` matches your controller (default is CC2)
   - `VelMode` is set appropriately (`Default`, `BrOnly`, or `Hybrid`)
6. Play a note and send breath.

If there is no sound, jump to section 10.

## 2. Front Panel Controls

Windy1 uses two rotary encoders with push buttons:

- Top encoder rotate: move through menu items.
- Top encoder press: enter or confirm selected item.
- Bottom encoder rotate: change selected value.
- Bottom encoder press: go back.

Status LED gives quick activity/breath feedback.

## 3. Menu Navigation Basics

High-level flow:

1. Top menu contains patch selection, volume, patch edit groups, and system settings.
2. `Patch...` pages change sound design.
3. `System...` changes controller/behavior and global setup.

Practical tip:

- Use `System...` for controller behavior.
- Use patch pages for timbre and modulation.

## 4. System Settings (What They Mean)

Commonly used global settings:

- `BreathCC`: selects which MIDI CC number (1–11) that controls breath amplitude path. Note: CC6 is skipped in the range. If your controller sends breath on a different CC, the firmware cannot currently map to it; use a different breath CC on your controller if possible.
- `VelMode`:
  - `Default`: velocity can seed attack when breath is zero.
  - `BrOnly`: velocity does not affect amplitude; breath only.
  - `Hybrid`: higher of velocity or current breath can drive attack level.

### Legato Assist Settings

These are for controllers that send short NoteOff gaps between connected notes.

- `LegatoA`:
  - `Off`: current/legacy release-retrigger behavior.
  - `On`: enables deferred release logic.
- `LegHold` (ms): hold window used by timeout-based policies.
- `BreathTh` (0..127): breath level needed to defer a release; uses internal hysteresis where release occurs 4 points lower than the engage threshold to reduce chatter.
- `LegPol`:
  - `BreathOnl`: release only on breath drop below threshold.
  - `BrOrTime`: release on breath drop or hold timeout.
  - `TimeOnly`: release on hold timeout regardless of breath.

Defaults are compatibility-safe:

- `LegatoA=Off`
- `LegPol=BreathOnl`
- `BreathTh=10`
- `LegHold=40`

## 5. Controller Setup Recipes

### Recipe A: Typical Breath Controller (CC2)

1. `System... -> BreathCC = 2`
2. `VelMode = BrOnly` for pure breath amplitude
3. Keep `LegatoA = Off` unless you hear retrigger artifacts

### Recipe B: Controller Sending Breath on CC11

1. `System... -> BreathCC = 11`
2. Keep `VelMode` based on play style:
   - `BrOnly` for pure breath control
   - `Hybrid` for stronger tongued attacks

### Recipe C: Controller with NoteOff Gaps (Legato Smoothing)

1. `LegatoA = On`
2. `LegPol = BreathOnl`
3. `BreathTh = 10`
4. `LegHold = 40`

If notes hang too long:

1. Change `LegPol` to `BrOrTime`
2. Lower `LegHold` (for example 25-30)
3. Optionally lower `BreathTh`

## 6. Performance Tuning Workflow

Use this quick method to tune response live:

1. Set `BreathCC` correctly first.
2. Choose `VelMode`:
   - Start with `BrOnly` for wind-controller realism.
3. Only if needed, enable `LegatoA`.
4. Tune in this order:
   - `LegPol`
   - `BreathTh`
   - `LegHold`

Rule of thumb:

- If transitions retrigger too much: increase assist (On, higher threshold).
- If notes release too slowly: reduce hold or use `BrOrTime`.

## 7. Save and Persistence Behavior

- System values (BreathCC, VelMode, LegatoA, etc.) are EEPROM-backed and persist across power cycles.
- Patch values (oscillators, filters, etc.) remain patch-scoped and load/save with each patch.
- Effects can be configured in two modes: `FXSource=Patch` (effects settings save with patch) or `FXSource=Global` (effects settings apply across all patches and must be saved explicitly via `Fx... > SaveGlobalFx`).
- After changing system behavior, power-cycle once to verify persistence.

## 8. Live Gig Checklist

Before performance:

1. Confirm output and monitoring chain.
2. Confirm `BreathCC` for the current controller.
3. Confirm `VelMode` and `LegatoA` profile for your playing style.
4. Load known-good patch.
5. Play long tones and connected intervals.
6. Verify no stuck-note behavior.

## 9. Debug Build Usage

Use debug firmware when troubleshooting control paths:

- Environment: `windy1_teensy41_debug`
- Debug serial includes MIDI summaries, mismatch warnings, and NRPN traces.

Look for:

1. NoteOn/NoteOff activity.
2. Breath CC arriving on expected CC number.
3. Mismatch warnings if controller CC differs from `BreathCC`.

## 10. Troubleshooting

### No Sound

1. Confirm DAC wiring and power.
2. Raise `Volume`.
3. Confirm `BreathCC` matches controller.
4. Send note plus breath.
5. Check debug logs for incoming CC/Note events.

### Breath Not Responding

1. Controller may send a different CC.
2. Set `System... -> BreathCC` to matching CC.
3. Re-test with debug logs.

### Connected Notes Retriggering

1. Set `Key Trig` to `Single` in patch common settings.
2. Enable `LegatoA`.
3. Start with `LegPol=BreathOnl`, `BreathTh=10`, `LegHold=40`.

### Notes Hanging Too Long

1. Change `LegPol` to `BrOrTime`.
2. Lower `LegHold`.
3. Lower `BreathTh` slightly.

### No Attack When Starting Note

1. Check `VelMode`:
   - `BrOnly` requires active breath signal.
2. Try `Default` or `Hybrid` if your controller note timing needs velocity seeding.

## 11. Glossary

- BreathCC: MIDI CC number (1–11, excluding 6) used for breath input; must match the CC your controller sends.
- VelMode: how note velocity influences amplitude at note onset.
- LegatoA: legato assist enable switch.
- LegPol: legato release decision policy.
- BreathTh: breath threshold for assisted release logic.
- LegHold: timeout window used by timeout-capable policies.

## 12. Recommended Baseline Profiles

### Profile: Conservative / Legacy

- `BreathCC=2`
- `VelMode=Default`
- `LegatoA=Off`

### Profile: Wind Controller Natural

- `BreathCC=2` (or device-specific)
- `VelMode=BrOnly`
- `LegatoA=Off`

### Profile: Gap-Smoothing Assist

- `BreathCC=device-specific`
- `VelMode=BrOnly`
- `LegatoA=On`
- `LegPol=BreathOnl`
- `BreathTh=10`
- `LegHold=40`

---

If you are building hardware or validating wiring, continue with docs/WINDY1_HARDWARE_BUILD_GUIDE.md.
