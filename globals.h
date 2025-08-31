// globals.h
//
//#pragma once
#include <stdint.h>
#include <string>
#include <Audio.h>
#include <SD.h>
#include "OledMenu.h"
#include "patches.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMono9pt7b.h>

#define CC_MODULATION_WHEEL               1
#define CC_BREATH                         2
#define CC_FOOT_PEDAL                     4
#define CC_PORTA_TIME                     5
#define CC_VOLUME                         7
#define CC_BALANCE                        8
#define CC_PAN                            10
#define CC_EXPRESSION                     11
#define CC_NRPN_DATA_ENTRY                6
#define CC_NRPN_LSB                       98
#define CC_NRPN_MSB                       99

//------------------- ssd1306_128x32_i2c OLED --------------------------
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     33 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET); // in globals.cpp
extern Adafruit_SSD1306 display;  // MenuItemsListFuncs.cpp


extern int currentPatch;

#define HW_VERSION_DUAL_ENCODER
#ifdef HW_VERSION_DUAL_ENCODER
    // KNOB1
    extern const int knobTopButtonPin;  
    extern const int knobTopEncoderPin1;  
    extern const int knobTopEncoderPin2;  
    extern Encoder knobTop;
    extern Bounce knobTopButton;
    // KNOB2
    extern const int knobBotButtonPin;  
    extern const int knobBotEncoderPin1;  // teensy4.1 pin to KNOB2A
    extern const int knobBotEncoderPin2;  // teensy4.1 pin to KNOB2B
    extern Encoder knobBot;
    extern Bounce knobBotButton;
#else // old one knob version
    // KNOB2
    extern const int knobBotButtonPin;  
    extern const int knobBotEncoderPin1;  
    extern const int knobBotEncoderPin2;  
    extern Encoder knobBot;
    extern Bounce knobBotButton;
#endif
extern const int topButtonPin;  
extern const int botButtonPin;  

extern Bounce topButton;
extern Bounce botButton;

extern int32_t newKnob;

extern OledMenu myMenu;

// globls for midi
extern uint8_t type;
extern uint8_t data1;
extern uint8_t data2;  
extern uint8_t channel; 

extern uint8_t usbMidiNrpnLsbOld;
extern uint8_t usbMidiNrpnMsbOld;
extern uint8_t usbMidiNrpnLsbNew;
extern uint8_t usbMidiNrpnMsbNew;
extern uint8_t usbMidiNrpnData;

// globals for debugging
extern String verNum_str;
extern String verTxt_str; 
extern String splashTxt; 
extern String version_str;
extern String splashScreen_str;
extern bool PRINT_VALUES_FLAG;
extern char str_buf[];
extern char str_buf1[];

// globals for display
//extern char str_oledbuf[];
extern char str_namingbuf[];
extern const String alphaNumString;



// globals for loop control
extern uint32_t previousMillis;
extern uint32_t currentMillis;  
extern uint32_t previousUsageTime;
extern uint32_t previousUITimeoutTime;
extern uint32_t currentUITimeoutTime;
extern uint32_t previousDebugPrintTime;
extern bool ALREADY_TIMED_OUT;

// globals for synth control parametears
extern uint8_t currentMidiNote;  // A0
extern bool note_is_on;
extern float DIV8192;
extern float DIV127;
extern float DIV128;
extern float DIV64;
extern bool updateSynthVariablesFlag;
extern bool preUpdateSynthVariablesFlag;

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
extern float dc_portatime_gamma;  // TODO: match VL-70m
extern float dc_pitchbend_ramp_rate ;
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
extern int32_t vol;
extern int eeprom_vol;
extern float volf;
extern int32_t fxSourcePatch;
extern int eeprom_fxSourcePatch;
extern int32_t mix_linein;
extern int eeprom_mix_linein;
extern float mix_lineinf;
extern int32_t FineTuneCents;  	// 100 cents per halfstep, FineTune Control +/- 100
extern int eeprom_FineTuneCents;
extern float FineTuneCentsf;  	
extern int32_t Transpose;  	// semitones +/- 12 
extern int eeprom_Transpose;
extern float Transposef;  	
extern int32_t Octave;  	// octaves  +/- 2 
extern int eeprom_Octave;
extern float Octavef;  
extern int32_t breath_cc;  // can be 1 to 11 (except 06) for cc01, cc02, etc. (default cc02)
extern int eeprom_breath_cc;
extern std::vector<int32_t> breath_cc_choices[];

// synth variable limits and references
extern float maxPwmLfoFreq;         // 4000s is 10 Hz at 100%
extern float maxPwmDepthOsc;        // 4000s is +/- 25% at 100% depth
extern float maxSweepTimeOsc;       // TODO: set this to match 4000s
extern float sweepTimeOscGamma;       // TODO: adjust this to match 4000s
extern float maxSweepTimeOscFilter;
extern float sweepTimeOscFilterGamma;       // TODO: adjust this to match 4000s
extern float maxSweepTimeNoiseFilter;
extern float sweepTimeNoiseFilterGamma;       // TODO: adjust this to match 4000s
extern float maxSweepDepthFilter;   // 7/8 = 0.875 (because new max filter modulation is 8 octaves) TODO: set this to match 4000s
extern float maxSweepDepthOscFilter;  	//72,9,0,127,

//extern float WS_PWOSC_ARRAY[]; 

extern float maxLfoFreqFilter1;
extern float maxLfoFreqFilter2;
extern float maxLfoFreqFilter3;
extern float maxLfoFreqFilter4;
extern float maxLfoDepthOscFilter1;  // 4000s goes up and down about 2 octaves (so 0.28 should be right, but seems too much)
extern float maxLfoDepthOscFilter2;  // 4000s goes up and down about 2 octaves 
extern float maxLfoDepthNoiseFilter3;  // 4000s goes up and down about 2 octaves 
extern float maxLfoDepthNoiseFilter4;  // 4000s goes up and down about 2 octaves 
extern float modOffsetFilter1;    // -0.25, Hack to start filter at lower frequency
extern float modOffsetFilter2;
extern float modOffsetFilter3;
extern float modOffsetFilter4;
extern float clippedFreqFilter1;    // C6
extern float clippedFreqFilter2;    // C6
extern float clippedFreqFilter3;    // C6
extern float clippedFreqFilter4;    // C6
extern float clippedFreqFilterPreNoise;    // C6
extern float filterPreNoiseFreqFactor;    // C6
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
extern float offsetNoteKeyfollow;  // 84 = C6, 72 = C5, 60 = C4
extern float offsetNoteKeyfollowFilter5; // 84 = C6, 72 = C5, 60 = C4
extern float offsetNoteKeyfollowNoise;  // 84 = C6, 72 = C5, 60 = C4
extern float offsetNoteKeyfollowPreNoise;  // 84 = C6, 72 = C5, 60 = C4
extern float minPreNoiseNoteNumbr;  // 84 = C6, 72 = C5, 60 = C4  4000s noise stops changing below about C4
extern const uint32_t updatedAudioSystemInterval;  // milliseconds
extern float freqOscFilterOctaveOffset ;    // use 3 to divide FreqOscFilterN by 2^3 to allow modulation to go from -1 to +3/7 
//float fOFOFactor; 
extern float maxFilterFreq; 
extern float minFilterFreq; // min note number 36
extern float minFilterPreNoiseFreq; // middle C (C4)
extern float FreqPreNoiseVariableFreq; // TODO: match 4000s
extern float maxDelayDamp; //TODO: find out correct value
extern float maxDelayLevel; //TODO: find out correct value
extern float gammaDelayLevel; //TODO: find out correct value
extern float gammaDelayFeedback; //TODO: find out correct value
extern float maxTimeNoise;  // 1000 ms
extern float TimeNoiseGamma;  
extern float maxNoiseLevel;  
extern float logPotYmidLevelNoise;
extern float minGamma;
extern float maxGamma;
extern float maxReverbLevel;
extern float maxDenseEarly;
extern bool  porta_step;    // round to nearest note for portamento = glissando  // Not really in EWI 4k, but I like it :)
extern float octaveControlOsc1;
extern float octaveControlOsc2;
extern float limitBreathSweepOsc1;  // 4000s is about 1.55 semitiones 
extern float limitBreathSweepOsc2; // 4000s is about 1.55 semitiones  
extern float maxMixNtcFilter; 
extern float extraAmpFactor;
extern float extraLineInAmpFactor;
extern float mix_lineInLR_gain_0;
extern float mix_lineInLR_gain_1;


// current patch nrpn numbers
extern patch_t current_patch;  // 154 bytes long
extern patch_t copy_buffer_patch;  // 154 bytes long
extern patch_t swap_buffer_patch;  // 154 bytes long
extern fx_t global_buffer_fx;  // 61 bytes long
extern fx_t copy_buffer_fx; 

extern const uint32_t eepromUpdateInterval;  // milliseconds
extern uint32_t eepromPreviousMillis;
extern uint32_t eepromCurrentMillis;
extern bool programChangeFlag; // new programChange is happening
extern int programChangeData; // value from midi_ho
extern bool updateEpromFlag;
extern int current_patchNumber;
extern int paste_patchNumber;
extern int swap_patchNumber;
extern int sourcePatchNumber;
extern int eeprom_patchNumber;
extern int paste_FxNumber;
extern patch_t loadedPatches[];
extern bool patchLoaded[];
extern bool globalFxLoaded;
extern File dataFile;
extern uint8_t sysexPresetSendBuffer[];

typedef enum { LP, HP, BP, NTC, THRU } filterMode_t;

// patch synth variables
extern float OctOsc1;      //64,0,62,66,
extern float SemiOsc1;     //64,1,52,76, -12 to +12
extern float FineOsc1;  	//64,2,14,114, -50cents/100 to +50cents/100
extern float BeatOsc1;  	//64,3,0,127, 0 to 1.0
extern float BeatMax;  	
extern float SawOsc1;  	//64,5,0,127, 0 to 1.0
extern float logPotYmidWaveN;
extern float TriOsc1;  	//64,6,0,127, 0 to 1.0
extern float PulseOsc1;  	//64,7,0,127, 0 to 1.0
extern float PwOffsetOsc1;  	//64,8,0,127, 0 to 1.0
extern float PwOsc1;  	//64,8,0,127, 0 to 1.0
extern float PwmFreqOsc1;   	//64,9,0,127,   // 0.0 to 1.0 * maxPwmLfoFreq 
extern float PwmDepthOsc1;  	//64,10,0,127,
extern float SweepDepthOsc1;  	//64,11,0,127,
extern float SweepTimeOsc1;  	//64,12,0,127,
extern float BreathDepthOsc1;  	//64,13,0,127, <64 start flat, >64 start sharp, -1.0 to +1.0
extern float BreathAttainOsc1;  	//64,14,0,127,
struct CurveLines {
    float midIntercept;
    float loSlope;
    float hiSlope;
};
extern float BreathCurveOsc1;  	//64,15,0,127,
extern CurveLines BreathOscCurveLines1;
extern float BreathThreshOsc1;  	//64,16,0,127,
extern float LevelOsc1;  		//64,17,0,127,
extern float LimiterAmount;
extern float LevelOscN_HeadRoom;  		//64,17,0,127,
extern float logPotYmidLevelOscN;
extern float Amp_HeadRoom;  		//64,17,0,127,
extern float OctOsc2;  	//65,0,62,66,
extern float SemiOsc2;  	//65,1,52,76,
extern float FineOsc2;  	//65,2,14,114,
extern float BeatOsc2;  	//65,3,0,127,
extern float SawOsc2;  	//65,5,0,127,
extern float TriOsc2;  	//65,6,0,127,
extern float PulseOsc2;  	//65,7,0,127,
extern float PwOffsetOsc2;  	//64,8,0,127, 0 to 1.0
extern float PwOsc2;  	//65,8,0,127,
extern float PwmFreqOsc2;  	//65,9,0,127,
extern float PwmDepthOsc2;  	//65,10,0,127,
extern float SweepDepthOsc2;  	//65,11,0,127,
extern float SweepTimeOsc2;  	//65,12,0,127,
extern float BreathDepthOsc2;  	//65,13,0,127,
extern float BreathAttainOsc2;  	//65,14,0,127,
extern float BreathCurveOsc2;  	//65,15,0,127, +63, -64, so 1+(x-64)*DIV64
extern CurveLines BreathOscCurveLines2;
extern float BreathThreshOsc2;  	//65,16,0,127,
extern float LevelOsc2;  	//65,17,0,127,
extern filterMode_t ModeOscFilter1;  	//72,0,0,4,//LP HP BP NTC OFF
extern float FreqOscFilter1 ;    // C6  	//72,1,36,124,//Midi Note 36 to 124
extern float FreqOscFilter1BModFactor; // Freq slider Note numeber 36 to 124
extern float QFactorOscFilter1;  	//72,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowOscFilter1;  	//72,3,52,88,// -12 to +24 num semi tones
extern float BreathModOscFilter1;  	//72,4,0,127,
extern float LfoFreqOscFilter1;  	//72,5,0,127,
extern float LfoDepthOscFilter1;  	//72,6,0,127,
extern float LfoBreathOscFilter1;  	//72,7,0,127,
extern float LfoThreshOscFilter1;  	//72,8,0,127,
extern float SweepDepthOscFilter1;  	//72,9,0,127,
extern float SweepTimeOscFilter1;  	//72,10,0,127,
extern float BreathCurveOscFilter1;  	//72,11,0,127,  TODO: hook this up
extern CurveLines BreathOscFiltCurveLines1;
extern filterMode_t ModeOscFilter2;  	//73,0,0,4,//LP HP BP NTC OFF
extern filterMode_t ModeOscFilter2_stored;  	//73,0,0,4,//LP HP BP NTC OFF
extern float FreqOscFilter2;    // C6  	//73,1,36,124,//Midi Note 36 to 124
extern float FreqOscFilter2BModFactor; // Freq slider Note numeber 36 to 124
extern float QFactorOscFilter2;  	//73,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowOscFilter2;  	//73,3,52,88,// -12 to +24 num semi tones
extern float BreathModOscFilter2;  	//73,4,0,127,
extern float LfoFreqOscFilter2;  	//73,5,0,127,
extern float LfoDepthOscFilter2;  	//73,6,0,127,
extern float LfoBreathOscFilter2;  	//73,7,0,127,
extern float LfoThreshOscFilter2;  	//73,8,0,127,
extern float SweepDepthOscFilter2;  	//73,9,0,127,
extern float SweepTimeOscFilter2;  	//73,10,0,127,
extern float BreathCurveOscFilter2;  	//73,11,0,127, TODO: hook this up
extern CurveLines BreathOscFiltCurveLines2;
extern filterMode_t ModeNoiseFilter3;  	//74,0,0,4,//LP HP BP NTC OFF
extern float FreqNoiseFilter3;    // C6  	//74,1,36,124,//Midi Note 36 to 124
extern float FreqNoiseFilter3BModFactor; // Freq slider Note numeber 36 to 124
extern float QFactorNoiseFilter3;  	//74,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowNoiseFilter3;  	//74,3,52,88,// -12 to +24 num semi tones
extern float BreathModNoiseFilter3;  	//74,4,0,127,
extern float LfoFreqNoiseFilter3;  	//74,5,0,127,
extern float LfoDepthNoiseFilter3;  	//74,6,0,127,
extern float LfoBreathNoiseFilter3;  	//74,7,0,127,
extern float LfoThreshNoiseFilter3;  	//74,8,0,127,
extern float SweepDepthNoiseFilter3;  	//74,9,0,127,
extern float SweepTimeNoiseFilter3;  	//74,10,0,127,
extern float BreathCurveNoiseFilter3;  	//74,11,0,127, TODO: hook this up
extern CurveLines BreathNoiseFiltCurveLines3;
extern filterMode_t ModeNoiseFilter4;  	//75,0,0,4,//LP HP BP NTC OFF
extern filterMode_t ModeNoiseFilter4_stored;  	//75,0,0,4,//LP HP BP NTC OFF
extern float FreqNoiseFilter4;    // C6  	//75,1,36,124,//Midi Note 36 to 124
extern float FreqNoiseFilter4BModFactor; // Freq slider Note numeber 36 to 124
extern float QFactorNoiseFilter4;  	//75,2,5,127,"// 5=0.5, 127=12.7"
extern float QFactorFilter5;  	//75,2,5,127,"// 5=0.5, 127=12.7"
extern float KeyFollowNoiseFilter4;  	//75,3,52,88,// -12 to +24 num semi tones
extern float KeyFollowFilter5;  	//75,3,52,88,// -12 to +24 num semi tones
extern float BreathModNoiseFilter4;  	//75,4,0,127,
extern float LfoFreqNoiseFilter4;  	//75,5,0,127,
extern float LfoDepthNoiseFilter4;  	//75,6,0,127,
extern float LfoBreathNoiseFilter4;  	//75,7,0,127,
extern float LfoThreshNoiseFilter4;  	//75,8,0,127,
extern float SweepDepthNoiseFilter4;  	//75,9,0,127,
extern float SweepTimeNoiseFilter4;  	//75,10,0,127,
extern float BreathCurveNoiseFilter4;  	//75,11,0,127, TODO: hook this up
extern CurveLines BreathNoiseFiltCurveLines4;
extern float KeyFollowPreNoiseFilter;  // TODO: match 4000s
extern float keyfollowFilterPreNoise; 
extern float FreqPreNoiseFilter; // TODO: match 4000s
extern float NoiseTime;  	//80,0,0,127,
extern float TimeNoiseAmp;  	//80,0,0,127,
extern float NoiseBreathCurve;  	//80,1,0,127,
extern CurveLines NoiseBreathCurveLines;
extern float NoiseLevel;  	//80,2,0,127,
extern float BendRange;  	//81,0,0,12,// num semitones
extern bool BendStep;  	//81,1,0,1,//0=off 1=on
extern float VibratoPitch;  	//81,2,0,127,
extern bool LinkOscFilters;  	//81,3,1,2,//1=Link on 2= Link off  TODO: hook this up
extern bool LinkNoiseFilters;  	//81,4,1,2,                         TODO: hook this up
extern uint8_t Formant;  	        //81,5,0,2,//0=off 1=W.Wind 2=String
extern bool XFade;  	        //81,6,0,1,//0=off 1=on
extern bool KeyTriggerSingle;  	    //81,7,0,1,//0=Multi 1=Single
extern float s81_8;
extern bool ChorusOn;
extern float VibratoAmp;  	    //88,0,0,127,
extern float AmpLevel;  	    //88,1,0,127,
extern float mix_Amp_gain_0;
extern float mix_Amp_gain_1;
extern float OctButtonLevel;  	    //88,2,0,127,
extern float EffectsChorusDelay1;  //112,0,0,127,
extern float EffectsChorusMod1;  	//112,1,0,127,
extern float EffectsChorusWet1;  	//112,2,0,127,
extern float EffectsChorusDelay2;  //112,3,0,127,
extern float EffectsChorusMod2;  	//112,4,0,127,
extern float EffectsChorusWet2;  	//112,5,0,127,
extern float EffectsChorusFeedback;//112,6,0,127,
extern float EffectsChorusLfoFreq; //112,7,0,127,
extern float EffectsChorusDryLevel;//112,8,0,127,
extern float EffectsDelayTimeL;  	//113,0,0,127,
extern float EffectsDelayTimeR;  	//113,0,0,127,
extern float EffectsDelayFeedback; //113,1,0,127,
extern float EffectsDelayDamp;  	//113,2,0,127,
extern float EffectsDelayLevel;  	//113,3,0,127,
extern float EffectsDelayPong;  	//113,3,0,127,
extern float EffectsReverbSpare;  	//114,1,0,127,
extern float EffectsReverbLevel;  	//114,1,0,127,
extern float EffectsReverbDenseEarly;  	//114,2,0,127,
extern float EffectsReverbTime;  	//114,3,10,50,//1.0 to 5.0 sec
extern float EffectsReverbDamp;  	//114,4,54,74,//-10 to +10
extern float EffectsChorusFBHeadroom; // to reduce levels of chorus fb mixer to keep from clipping
extern float EffectsChorusDryHeadroom; // to reduce levels of chorus dry mixer to keep from clipping

// Number of samples in each delay line
// AUDIO_BLOCK_SAMPLES = 128
// 4k GUI says 127 mS max delay = 44 blocks at 44.1khz but flange cuts delay in half
// 44.1 samples/ms, so 88.2 samples/ms when using flange
#define FLANGE_DELAY_LENGTH (176*AUDIO_BLOCK_SAMPLES)
// Allocate the delay lines for left and right channels
extern short delayline_flange1[];
extern short delayline_flange2[];
extern short delayline_flange3[];

extern filterMode_t modeFilter1_old;
extern filterMode_t modeFilter2_old;
extern filterMode_t modeFilter3_old;
extern filterMode_t modeFilter4_old;


