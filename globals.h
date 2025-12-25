// globals.h
#ifndef globals_h_
#define globals_h_

#include <Arduino.h>
#include <AudioStream.h>
#include <SD.h>
#include "patches.h"

// globals for midi
#define CC_MODULATION_WHEEL 1
#define CC_BREATH 2
#define CC_FOOT_PEDAL 4
#define CC_PORTA_TIME 5
#define CC_VOLUME 7
#define CC_BALANCE 8
#define CC_PAN 10
#define CC_EXPRESSION 11
#define CC_NRPN_DATA_ENTRY 6
#define CC_NRPN_LSB 98
#define CC_NRPN_MSB 99

extern uint8_t type;
extern uint8_t data1;
extern uint8_t data2;
extern uint8_t channel;

extern uint8_t usbMidiNrpnLsbOld;
extern uint8_t usbMidiNrpnMsbOld;
extern uint8_t usbMidiNrpnLsbNew;
extern uint8_t usbMidiNrpnMsbNew;
extern uint8_t usbMidiNrpnData;

// plug sensing
extern bool monoModeFlag;

// globals for debugging
extern String verNum_str;
extern String verTxt_str;
extern String splashTxt;
extern String version_str;
extern String splashScreen_str;
extern bool SPLASH_SCREEN_ON;
extern bool PRINT_VALUES_FLAG;
extern char str_buf[];
extern char str_buf1[];
extern char namebuf[]; // naming buffer
extern char str_oledbuf[];
extern bool namingFinishedFlag;

// globals for loop control
extern uint32_t previousMillis;
extern uint32_t currentMillis;
extern uint32_t previousUsageTime;
extern uint32_t previousUITimeoutTime;
extern uint32_t currentUITimeoutTime;
extern uint32_t previousDebugPrintTime;
extern bool ALREADY_TIMED_OUT;

// globals for synth control parameters
extern uint8_t currentMidiNote; // A0
extern bool note_is_on;
extern float DIV8192;
extern float DIV127;
extern float DIV128;
extern float DIV64;
extern bool updateSynthVariablesFlag;
extern bool preUpdateSynthVariablesFlag;
extern bool updateCalibrationFlag;

// globals for synth
extern float dc_breathOscFilter1_amp;
extern float dc_breathOscFilter2_amp;
extern float dc_breathNoiseFilter3_amp;
extern float dc_breathNoiseFilter4_amp;
extern float dc_breathFilterN_rampTime;
extern float dc_breathNoise_amp;
extern float dc_breathNoise_rampTime;
extern float dc_breathThreshOsc1_amp;
extern float dc_breathThreshOsc2_amp;
extern float dc_breathSweepOscN_rampTime;
extern float dc_breathThreshNoise_amp;
extern float dc_breathThreshOscN_rampTime;
extern float dc_breathOff_rampTime;
extern float dc_breathLfoFilter1_amp;
extern float dc_breathLfoFilter2_amp;
extern float dc_breathLfoFilter3_amp;
extern float dc_breathLfoFilter4_amp;
extern float dc_breathLfoFilter_rampTime;
extern float dc_portatime_val;
extern float dc_portatime_range; // number of ms when _val is 1.0
extern float dc_portatime_gamma;
extern float dc_pitchbend_ramp_rate;
extern float noteNumberOsc1;
extern float noteFreqOsc1;
extern float noteNumberOsc2;
extern float noteFreqOsc2;
extern float noteFreqFilterOsc1; // note for hp filter for each osc
extern float noteFreqFilterOsc2;
extern float noteFreqFilter5;
extern float noteNumberFilter1;
extern float noteNumberFilter2;
extern float portatimef_t;
extern float portatime_temp;
extern float portatime_min;
extern float fMidiNoteNorm;
extern float fMidiNoteNorm_diff;
extern float data2f;
extern float data1f;
extern float lastBreathf;
extern float previousBreathf;
extern uint8_t vol;
extern int eeprom_vol;
extern float volf;
extern uint8_t fxSourcePatch;
extern int eeprom_fxSourcePatch;
extern uint8_t mix_linein;
extern int eeprom_mix_linein;
extern float mix_lineinf;
extern uint8_t FineTuneCents; // 100 cents per halfstep, FineTune Control +/- 100
extern int eeprom_FineTuneCents;
extern float FineTuneCentsf;
extern uint8_t Transpose; // semitones +/- 12
extern int eeprom_Transpose;
extern float Transposef;
extern uint8_t Octave; // octaves  +/- 2
extern int eeprom_Octave;
extern float Octavef;
extern uint8_t breath_cc;      // can be 1 to 11 (except 06) for cc01, cc02, etc. (default cc02)
extern uint8_t breath_cc_last; // can be 1 to 11 (except 06) for cc01, cc02, etc. (default cc02)
extern int eeprom_breath_cc;
extern std::vector<int32_t> breath_cc_choices[];

/*
    It offers the following controls:
    Attack rates 0.1 - 2000 mS, float; but using 0.1-12.7ms (1-127), stepsize 0.1
    Release rates 0.1 - 2000mS, float; but using 1-127ms, stepsize 1
    Ratio between 1 to 32767, float; but using 0.1-12.7 (1-127), stepsize 0.1
    Knee width between 0 to 40dB (soft to hard knee control), float; using 0-40, stepsize 1
    Threshold between 0 to -40dB, float, using 0 to -40 (24-64) stepsize 1
    Makeup Gain between 0 to 40dB, float, using 0 to 40, stepsize 1
    Multiple side chain inputs that can be selected at run time, uint8_t, 0 or 1
*/
#define EEPROM_COMP_PARAMS_SIZE 6
extern int eeprom_comp_params[EEPROM_COMP_PARAMS_SIZE];
extern uint8_t comp_params[EEPROM_COMP_PARAMS_SIZE];
extern float comp_paramsf[EEPROM_COMP_PARAMS_SIZE];
extern uint8_t comp_sideChain;
extern int eeprom_NNBModCal;

// patch synth variables
extern float OctOsc1;  // 64,0,62,66,
extern float SemiOsc1; // 64,1,52,76, -12 to +12
extern float FineOsc1; // 64,2,14,114, -50cents/100 to +50cents/100
extern float BeatOsc1; // 64,3,0,127, 0 to 1.0
extern float BeatMax;
extern float SawOsc1; // 64,5,0,127, 0 to 1.0
extern float logPotYmidWaveN;
extern float TriOsc1;          // 64,6,0,127, 0 to 1.0
extern float PulseOsc1;        // 64,7,0,127, 0 to 1.0
extern float PwOffsetOsc1;     // 64,8,0,127, 0 to 1.0
extern float PwOsc1;           // 64,8,0,127, 0 to 1.0
extern float PwmFreqOsc1;      // 64,9,0,127,   // 0.0 to 1.0 * maxPwmLfoFreq
extern float PwmDepthOsc1;     // 64,10,0,127,
extern float SweepDepthOsc1;   // 64,11,0,127,
extern float SweepTimeOsc1;    // 64,12,0,127,
extern float BreathDepthOsc1;  // 64,13,0,127, <64 start flat, >64 start sharp, -1.0 to +1.0
extern float BreathAttainOsc1; // 64,14,0,127,
struct CurveLines
{
  float midIntercept;
  float loSlope;
  float hiSlope;
};
extern float BreathCurveOsc1; // 64,15,0,127,
extern CurveLines BreathOscCurveLines1;
extern float BreathThreshOsc1; // 64,16,0,127,
extern float LevelOsc1;        // 64,17,0,127,
extern float LimiterAmount;
extern float LevelOscN_HeadRoom; // 64,17,0,127,
extern float logPotYmidLevelOscN;
extern float Amp_HeadRoom;     // 64,17,0,127,
extern float OctOsc2;          // 65,0,62,66,
extern float SemiOsc2;         // 65,1,52,76,
extern float FineOsc2;         // 65,2,14,114,
extern float BeatOsc2;         // 65,3,0,127,
extern float SawOsc2;          // 65,5,0,127,
extern float TriOsc2;          // 65,6,0,127,
extern float PulseOsc2;        // 65,7,0,127,
extern float PwOffsetOsc2;     // 64,8,0,127, 0 to 1.0
extern float PwOsc2;           // 65,8,0,127,
extern float PwmFreqOsc2;      // 65,9,0,127,
extern float PwmDepthOsc2;     // 65,10,0,127,
extern float SweepDepthOsc2;   // 65,11,0,127,
extern float SweepTimeOsc2;    // 65,12,0,127,
extern float BreathDepthOsc2;  // 65,13,0,127,
extern float BreathAttainOsc2; // 65,14,0,127,
extern float BreathCurveOsc2;  // 65,15,0,127, +63, -64, so 1+(x-64)*DIV64
extern CurveLines BreathOscCurveLines2;
extern float BreathThreshOsc2;         // 65,16,0,127,
extern float LevelOsc2;                // 65,17,0,127,
extern filterMode_t ModeOscFilter1;    // 72,0,0,4,//LP HP BP NTC OFF
extern float FreqOscFilter1;           // C6  	//72,1,36,124,//Midi Note 36 to 124
extern float FreqOscFilter1BModFactor; // Freq slider Note numeber 36 to 124
extern float QFactorOscFilter1;        // 72,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowOscFilter1;      // 72,3,52,88,// -12 to +24 num semi tones
extern float BreathModOscFilter1;      // 72,4,0,127,
extern float LfoFreqOscFilter1;        // 72,5,0,127,
extern float LfoDepthOscFilter1;       // 72,6,0,127,
extern float LfoBreathOscFilter1;      // 72,7,0,127,
extern float LfoThreshOscFilter1;      // 72,8,0,127,
extern float SweepDepthOscFilter1;     // 72,9,0,127,
extern float SweepTimeOscFilter1;      // 72,10,0,127,
extern float BreathCurveOscFilter1;    // 72,11,0,127,  // using BreathOscFilterCurveLines1 instead
extern CurveLines BreathOscFiltCurveLines1;
extern filterMode_t ModeOscFilter2;        // 73,0,0,4,//LP HP BP NTC OFF
extern filterMode_t ModeOscFilter2_stored; // 73,0,0,4,//LP HP BP NTC OFF
extern float FreqOscFilter2;               // C6  	//73,1,36,124,//Midi Note 36 to 124
extern float FreqOscFilter2BModFactor;     // Freq slider Note numeber 36 to 124
extern float QFactorOscFilter2;            // 73,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowOscFilter2;          // 73,3,52,88,// -12 to +24 num semi tones
extern float BreathModOscFilter2;          // 73,4,0,127,
extern float LfoFreqOscFilter2;            // 73,5,0,127,
extern float LfoDepthOscFilter2;           // 73,6,0,127,
extern float LfoBreathOscFilter2;          // 73,7,0,127,
extern float LfoThreshOscFilter2;          // 73,8,0,127,
extern float SweepDepthOscFilter2;         // 73,9,0,127,
extern float SweepTimeOscFilter2;          // 73,10,0,127,
extern float BreathCurveOscFilter2;        // 73,11,0,127, // using BreathOscFilterCurveLines2 instead
extern CurveLines BreathOscFiltCurveLines2;
extern filterMode_t ModeNoiseFilter3;    // 74,0,0,4,//LP HP BP NTC OFF
extern float FreqNoiseFilter3;           // C6  	//74,1,36,124,//Midi Note 36 to 124
extern float FreqNoiseFilter3BModFactor; // Freq slider Note numeber 36 to 124
extern float QFactorNoiseFilter3;        // 74,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowNoiseFilter3;      // 74,3,52,88,// -12 to +24 num semi tones
extern float BreathModNoiseFilter3;      // 74,4,0,127,
extern float LfoFreqNoiseFilter3;        // 74,5,0,127,
extern float LfoDepthNoiseFilter3;       // 74,6,0,127,
extern float LfoBreathNoiseFilter3;      // 74,7,0,127,
extern float LfoThreshNoiseFilter3;      // 74,8,0,127,
extern float SweepDepthNoiseFilter3;     // 74,9,0,127,
extern float SweepTimeNoiseFilter3;      // 74,10,0,127,
extern float BreathCurveNoiseFilter3;    // 74,11,0,127, // using BreathNoiseFilterCurveLines3 instead
extern CurveLines BreathNoiseFiltCurveLines3;
extern filterMode_t ModeNoiseFilter4;        // 75,0,0,4,//LP HP BP NTC OFF
extern filterMode_t ModeNoiseFilter4_stored; // 75,0,0,4,//LP HP BP NTC OFF
extern float FreqNoiseFilter4;               // C6  	//75,1,36,124,//Midi Note 36 to 124
extern float FreqNoiseFilter4BModFactor;     // Freq slider Note numeber 36 to 124
extern float QFactorNoiseFilter4;            // 75,2,5,127,"// 5=0.5, 127=12.7"
extern float QFactorFilter5;                 // 75,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowNoiseFilter4;          // 75,3,52,88,// -12 to +24 num semi tones
extern float KeyFollowFilter5;               // 75,3,52,88,// -12 to +24 num semi tones
extern float BreathModNoiseFilter4;          // 75,4,0,127,
extern float LfoFreqNoiseFilter4;            // 75,5,0,127,
extern float LfoDepthNoiseFilter4;           // 75,6,0,127,
extern float LfoBreathNoiseFilter4;          // 75,7,0,127,
extern float LfoThreshNoiseFilter4;          // 75,8,0,127,
extern float SweepDepthNoiseFilter4;         // 75,9,0,127,
extern float SweepTimeNoiseFilter4;          // 75,10,0,127,
extern float BreathCurveNoiseFilter4;        // 75,11,0,127, // using BreathNoiseFilterCurveLines4 instead
extern CurveLines BreathNoiseFiltCurveLines4;
extern float KeyFollowPreNoiseFilter;
extern float keyfollowFilterPreNoise;
extern float FreqPreNoiseFilter;
extern float NoiseTime;        // 80,0,0,127,
extern float TimeNoiseAmp;     // 80,0,0,127,
extern float NoiseBreathCurve; // 80,1,0,127,
extern CurveLines NoiseBreathCurveLines;
extern float NoiseLevel;      // 80,2,0,127,
extern float BendRange;       // 81,0,0,12,// num semitones
extern bool BendStep;         // 81,1,0,1,//0=off 1=on
extern float VibratoPitch;    // 81,2,0,127,
extern bool LinkOscFilters;   // 81,3,1,2,//1=Link on 2= Link off
extern bool LinkNoiseFilters; // 81,4,1,2,
extern uint8_t Formant;       // 81,5,0,2,//0=off 1=W.Wind 2=String
extern bool XFade;            // 81,6,0,1,//0=off 1=on
extern bool KeyTriggerSingle; // 81,7,0,1,//0=Multi 1=Single
extern float s81_8;
extern uint8_t ChorusOn;
extern float VibratoAmp; // 88,0,0,127,
extern float AmpLevel;   // 88,1,0,127,
extern float mix_Amp_gain_0;
extern float mix_Amp_gain_1;
extern float OctButtonLevel;           // 88,2,0,127,
extern float EffectsChorusDelay1;      // 112,0,0,127,
extern float EffectsChorusMod1;        // 112,1,0,127,
extern float EffectsChorusWet1;        // 112,2,0,127,
extern float EffectsChorusDelay2;      // 112,3,0,127,
extern float EffectsChorusMod2;        // 112,4,0,127,
extern float EffectsChorusWet2;        // 112,5,0,127,
extern float EffectsChorusFeedback;    // 112,6,0,127,
extern float EffectsChorusLfoFreq;     // 112,7,0,127,
extern float EffectsChorusDryLevel;    // 112,8,0,127,
extern float EffectsDelayTimeL;        // 113,0,0,127,
extern float EffectsDelayTimeR;        // 113,0,0,127,
extern float EffectsDelayFeedback;     // 113,1,0,127,
extern float EffectsDelayDamp;         // 113,2,0,127,
extern float EffectsDelayLevel;        // 113,3,0,127,
extern float EffectsDelayPong;         // 113,3,0,127,
extern float EffectsReverbSpare;       // 114,1,0,127,
extern float EffectsReverbLevel;       // 114,1,0,127,
extern float EffectsReverbDenseEarly;  // 114,2,0,127,
extern float EffectsReverbTime;        // 114,3,10,50,//1.0 to 5.0 sec
extern float EffectsReverbDamp;        // 114,4,54,74,//-10 to +10
extern float EffectsChorusFBHeadroom;  // to reduce levels of chorus fb mixer to keep from clipping
extern float EffectsChorusDryHeadroom; // to reduce levels of chorus dry mixer to keep from clipping

// synth variable limits and references
extern float maxPwmLfoFreq;  // 4000s is 10 Hz at 100%
extern float maxPwmDepthOsc; // 4000s is +/- 25% at 100% depth
extern float maxSweepTimeOsc;
extern float sweepTimeOscGamma;
extern float maxSweepTimeOscFilter;
extern float sweepTimeOscFilterGamma;
extern float maxSweepTimeNoiseFilter;
extern float sweepTimeNoiseFilterGamma;
extern float maxSweepDepthOscFilter; // 72,9,0,127,
extern float maxLfoFreqFilter1;
extern float maxLfoFreqFilter2;
extern float maxLfoFreqFilter3;
extern float maxLfoFreqFilter4;
extern float maxLfoDepthOscFilter1;   // 4000s goes up and down about 2 octaves (so 0.28 should be right, but seems too much)
extern float maxLfoDepthOscFilter2;   // 4000s goes up and down about 2 octaves
extern float maxLfoDepthNoiseFilter3; // 4000s goes up and down about 2 octaves
extern float maxLfoDepthNoiseFilter4; // 4000s goes up and down about 2 octaves
extern float modOffsetFilter1;        // -0.25, Hack to start filter at lower frequency
extern float modOffsetFilter2;
extern float modOffsetFilter3;
extern float modOffsetFilter4;
// extern float NN_BModF_one;
extern uint8_t NNBModCal;
extern float clippedFreqFilter1;        // C6
extern float clippedFreqFilter2;        // C6
extern float clippedFreqFilter3;        // C6
extern float clippedFreqFilter4;        // C6
extern float clippedFreqFilterPreNoise; // C6
extern float filterPreNoiseFreqFactor;  // C6
extern float keyfollowFilter1;
extern float keyfollowFilter2;
extern float keyfollowFilter3;
extern float keyfollowFilter4;
extern float keyfollowFilter5;
extern float octaveControlFilter1;
extern float octaveControlFilter2;
extern float octaveControlFilter3;
extern float octaveControlFilter4;
extern float octaveControlFilter5;
extern float octaveControlPreNoiseFilter;
extern float offsetNoteKeyfollow;                 // 84 = C6, 72 = C5, 60 = C4
extern float offsetNoteKeyfollowFilter5;          // 84 = C6, 72 = C5, 60 = C4
extern float offsetNoteKeyfollowNoise;            // 84 = C6, 72 = C5, 60 = C4
extern float offsetNoteKeyfollowPreNoise;         // 84 = C6, 72 = C5, 60 = C4
extern float minPreNoiseNoteNumbr;                // 84 = C6, 72 = C5, 60 = C4  4000s noise stops changing below about C4
extern const uint32_t updatedAudioSystemInterval; // milliseconds
extern float freqOscFilterOctaveOffset;           // use 3 to divide FreqOscFilterN by 2^3 to allow modulation to go from -1 to +3/7
// float fOFOFactor;
extern float maxFilterFreq;
extern float minFilterFreq;         // min note number 36
extern float minFilterPreNoiseFreq; // middle C (C4)
extern float FreqPreNoiseVariableFreq;
extern float maxDelayDamp;
extern float maxDelayLevel;
extern float gammaDelayLevel;
extern float gammaDelayFeedback;
extern float maxTimeNoise; // 1000 ms
extern float TimeNoiseGamma;
extern float maxNoiseLevel;
extern float logPotYmidLevelNoise;
extern float minGamma;
extern float maxGamma;
extern float maxReverbLevel;
extern float maxDenseEarly;
extern bool porta_step; // round to nearest note for portamento = glissando  // Not really in EWI 4k, but I like it :)
extern float octaveControlOsc1;
extern float octaveControlOsc2;
extern float limitBreathSweepOsc1; // 4000s is about 1.55 semitiones
extern float limitBreathSweepOsc2; // 4000s is about 1.55 semitiones
extern float maxMixNtcFilter;
extern float extraAmpFactor;
extern float extraLineInAmpFactor;
extern float mix_lineInLR_gain_0;
extern float mix_lineInLR_gain_1;

// Number of samples in each delay line
// AUDIO_BLOCK_SAMPLES = 128
// 4k GUI says 127 mS max delay = 44 blocks at 44.1khz but flange cuts delay in half
// 44.1 samples/ms, so 88.2 samples/ms when using flange
#define FLANGE_DELAY_LENGTH (176 * AUDIO_BLOCK_SAMPLES)
// Allocate the delay lines for left and right channels
extern short delayline_flange1[FLANGE_DELAY_LENGTH];
extern short delayline_flange2[FLANGE_DELAY_LENGTH];
extern short delayline_flange3[FLANGE_DELAY_LENGTH];

extern filterMode_t modeFilter1_old;
extern filterMode_t modeFilter2_old;
extern filterMode_t modeFilter3_old;
extern filterMode_t modeFilter4_old;

// current patch nrpn numbers
extern patch_t current_patch;     // 154 bytes long
extern patch_t copy_buffer_patch; // 154 bytes long
extern patch_t swap_buffer_patch; // 154 bytes long
extern fx_t global_buffer_fx;     // 61 bytes long
extern fx_t copy_buffer_fx;

extern const uint32_t eepromUpdateInterval; // milliseconds
extern uint32_t eepromPreviousMillis;
extern uint32_t eepromCurrentMillis;
extern bool programChangeFlag; // new programChange is happening
extern int programChangeData;  // value from midi_ho
extern bool updateEpromFlag;
extern int current_patchNumber;
extern int paste_patchNumber;
extern int paste_patchNumber_prev;
extern int swap_patchNumber;
extern int swap_patchNumber_prev;
extern int sourcePatchNumber;
extern int eeprom_patchNumber;
extern int paste_FxNumber;
extern patch_t loadedPatches[];
extern bool patchLoaded[];
extern bool globalFxLoaded;
extern File dataFile;
extern uint8_t sysexPresetSendBuffer[];

extern float gen_osc_gamma(float input);
extern float gen_filter_gamma(float input);
extern CurveLines gen_filter_curve_lines(float input);
extern CurveLines gen_osc_curve_lines(float input);
extern float piecewise_curve_func(float x, CurveLines fcl);
extern float log_pot_curve(float x, float Ymid);
extern float gen_noise_gamma(float input);
extern float gamma_func(float x, float gamma);
extern float amp_curve(float x);
extern float limit(float input, float top, float bottom);
extern float thresh(float x, float th);
extern float lfoThresh(float x, float th, float depth, float breath);

#endif
