// globals.cpp

#include <SD.h>
#include "OledMenu.h"
#include "globals.h"
#include "MenuItemsListsFuncs.h"
#include "patches.h"

// display
// Wire1: clk 16, data 17
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 16, /* data=*/ 17, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/ 16, /* data=*/ 17, /* reset=*/ U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_2ND_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// globals for Encoder knobs, buttons and UI state machine stuff----------------

const uint32_t debounceDelay = 10; //  ms
#ifdef HW_VERSION_DUAL_ENCODER
    // KNOB1
    const int knobTopButtonPin = 29;  // teensy4.1 pin
    const int knobTopEncoderPin1 = 31;  // teensy4.1 pin these encoders wired backwards?
    const int knobTopEncoderPin2 = 30;  // teensy4.1 pin
    Encoder knobTop(knobTopEncoderPin1, knobTopEncoderPin2);
    Bounce knobTopButton(knobTopButtonPin, debounceDelay);
    // KNOB2
    const int knobBotButtonPin = 32;  // teensy4.1 pin
    const int knobBotEncoderPin1 = 36;  // teensy4.1 pin to KNOB2B
    const int knobBotEncoderPin2 = 37;  // teensy4.1 pin to KNOB2A
    Encoder knobBot(knobBotEncoderPin1, knobBotEncoderPin2);
    Bounce knobBotButton(knobBotButtonPin, debounceDelay);
#else // old one knob version
    // KNOB2
    const int knobBotButtonPin = 32;  // teensy4.1 pin
    const int knobBotEncoderPin1 = 30;  // teensy4.1 pin
    const int knobBotEncoderPin2 = 31;  // teensy4.1 pin
    Encoder knobBot(knobBotEncoderPin1, knobBotEncoderPin2);
    Bounce knobBotButton(knobBotButtonPin, debounceDelay);
#endif
const int topButtonPin = 27;  // teensy4.1 pin
const int botButtonPin = 26;  // teensy4.1 pin
Bounce topButton(topButtonPin, debounceDelay);
Bounce botButton(botButtonPin, debounceDelay);

int32_t newKnob = 0;

// globals for midi
uint8_t type = 0;
uint8_t data1 = 0;
uint8_t data2 = 0;  
uint8_t channel = 0; 

uint8_t usbMidiNrpnLsbOld = 0;
uint8_t usbMidiNrpnMsbOld = 0;
uint8_t usbMidiNrpnLsbNew = 0;
uint8_t usbMidiNrpnMsbNew = 0;
uint8_t usbMidiNrpnData = 0;

// globals for debugging
String verNum_str = {"0.2.2"};
String verTxt_str = {"version: "}; 
String splashTxt = {"Windy 1\n  ver:\n   "}; 
String version_str = verTxt_str + verNum_str;
String splashScreen_str = {splashTxt + verNum_str};
char str_buf[64] = {0};
char str_buf1[64] = {0};
//char str_oledbuf[64] = {0}; // see OledMenu.h

bool PRINT_VALUES_FLAG = false;

// globals for loop control
uint32_t previousMillis = 0;
uint32_t currentMillis = 0;  
uint32_t previousUsageTime = 0;
uint32_t previousUITimeoutTime = 0;
uint32_t currentUITimeoutTime = 0;
uint32_t previousDebugPrintTime = 0;
bool ALREADY_TIMED_OUT = false;

// globals for synth control parameters
uint8_t currentMidiNote = 21;  // A0
bool note_is_on = false;
float DIV8192 = 1.0/8192.0;
float DIV127 = 1.0/127.0;
float DIV128 = 1.0/128.0;
float DIV64 = 1.0/64.0;
bool updateSynthVariablesFlag = false;
bool preUpdateSynthVariablesFlag = false;

// globals for synth
float dc_breathOscFilter1_amp = 0.0;
float dc_breathOscFilter2_amp = 0.0;
float dc_breathNoiseFilter3_amp = 0.0;
float dc_breathNoiseFilter4_amp = 0.0;
float dc_breathFilterN_rampTime = 8.0;
float dc_breathNoise_amp = 0.0;
float dc_breathNoise_rampTime =  0.0;
float dc_breathThreshOsc1_amp = 0.0;
float dc_breathThreshOsc2_amp = 0.0;
float dc_breathSweepOscN_rampTime = 4.0;
float dc_breathThreshNoise_amp = 0.0;
float dc_breathThreshOscN_rampTime = 4.0;
float dc_breathOff_rampTime =20.0;
float dc_breathLfoFilter1_amp = 0.0;
float dc_breathLfoFilter2_amp = 0.0;
float dc_breathLfoFilter3_amp = 0.0;
float dc_breathLfoFilter4_amp = 0.0;
float dc_breathLfoFilter_rampTime = 4.0;
float dc_portatime_val = 0.0;
float dc_portatime_range = 2500.0/12.0; // number of ms when _val is 1.0
float dc_portatime_gamma = 4.0;  // TODO: match VL-70m
float dc_pitchbend_ramp_rate  = 6.0f;
float noteNumberOsc1 = 69.0;  
float noteFreqOsc1 = 440.0;  
float noteNumberOsc2 = 69.0;  
float noteFreqOsc2 = 440.0;  
float noteFreqFilter5 = 220.0;  
float noteFreqFilterOsc1 = 68.0; // note for hp filter for each osc
float noteFreqFilterOsc2 = 68.0;
float noteNumberFilter1 = 69.0;  
float noteNumberFilter2 = 69.0;  
float portatimef_t = 6;
float portatime_temp= 0;
float portatime_min= 0;
float fMidiNoteNorm =69.0/128.0;  //440.0;  
float fMidiNoteNorm_diff = 0.0;
float data2f = 0.0;
float data1f = 0.0;
float lastBreathf = 0.0;
float previousBreathf = 0.0;
int32_t vol = 75;
int eeprom_vol = 75;
float volf = 1.5;
int32_t fxSourcePatch = 1; // 0 = global; 1 = patch
int eeprom_fxSourcePatch = 1;
int32_t mix_linein = 0;
int eeprom_mix_linein = 0;
float mix_lineinf = 0.0;
int32_t FineTuneCents = 0;  	// 100 cents per halfstep, FineTune Control +/- 100
int eeprom_FineTuneCents = 0;
float FineTuneCentsf = 0.0;  	
int32_t Transpose = 0;  	// semitones +/- 12 
int eeprom_Transpose = 0;
float Transposef = 0.0;  	
int32_t Octave = 0;  	// octaves  +/- 2 
int eeprom_Octave = 0;
float Octavef = 0.0;  
int32_t breath_cc = 2;  // can be 1, 2, 7 or 11 for cc01, cc02, etc. (default cc02)
int eeprom_breath_cc = 2;

// synth variable limits and references
float maxPwmLfoFreq = 5.6f;         // 4000s is 10 Hz at 100%
float maxPwmDepthOsc = 0.250;        // 4000s is +/- 25% at 100% depth
float maxSweepTimeOsc = 500.0;       // TODO: set this to match 4000s
float sweepTimeOscGamma = 3.50;       // TODO: adjust this to match 4000s
float maxSweepTimeOscFilter = 1000.0; // 255.0;
float sweepTimeOscFilterGamma = 4.0;       // TODO: adjust this to match 4000s
float maxSweepTimeNoiseFilter = 750.0;
float sweepTimeNoiseFilterGamma = 4.0;       // TODO: adjust this to match 4000s
float maxSweepDepthFilter = 1.0;   // 7/8 = 0.875 (because new max filter modulation is 8 octaves) TODO: set this to match 4000s
float maxSweepDepthOscFilter = 0.70;  	//72,9,0,127,

//float WAVESHAPER_ARRAY[] = {-16.0/16.0, -15.0/16.0, -14.0/16, -13.0/16.0, -12.0/16.0,-11.0/16.0, 
//                            -10.0/16.0, -9.0/16.0, -8.0/16.0, -7.0/16.0, -6.0/16.0, -5.0/16.0, 
//                            -4.0/16.0, -3.0/16.0, -2.0/16.0, -1.0/16.0, 0.0, 1.0/16.0, 2.0/16.0, 
//                            3.0/16.0, 4.0/16.0, 5.0/16.0, 6.0/16.0, 7.0/16.0, 8.0/16.0, 9.0/16.0, 
//                            10.0/16.0, 11.0/16.0, 12.0/16.0, 13.0/16.0, 13.5/16.0, 13.75/16.0, 14.0/16.0};
//float WS_PWOSC_ARRAY[] = {};


float maxLfoFreqFilter1 = 100.0;
float maxLfoFreqFilter2 = 100.0;
float maxLfoFreqFilter3 = 100.0;
float maxLfoFreqFilter4 = 100.0;
float maxLfoDepthOscFilter1 = 0.20; //0.15;  // 4000s goes up and down about 2 octaves (so 0.28 should be right, but seems too much)
float maxLfoDepthOscFilter2 = 0.20; //0.15;  // 4000s goes up and down about 2 octaves 
float maxLfoDepthNoiseFilter3 = 0.28;  // 4000s goes up and down about 2 octaves 
float maxLfoDepthNoiseFilter4 = 0.28;  // 4000s goes up and down about 2 octaves 
float modOffsetFilter1 = 1;    // -0.25, Hack to start filter at lower frequency
float modOffsetFilter2 = 1;
float modOffsetFilter3 = 1;
float modOffsetFilter4 = 1;
float clippedFreqFilter1 = 1046.5;    // C6
float clippedFreqFilter2 = 1046.5;    // C6
float clippedFreqFilter3 = 1046.5;    // C6
float clippedFreqFilter4 = 1046.5;    // C6
float clippedFreqFilterPreNoise = 600;    // C6
float filterPreNoiseFreqFactor = 4.0;    // C6
float keyfollowFilter1 = 1.0;
float keyfollowFilter2 = 1.0;
float keyfollowFilter3 = 1.0;
float keyfollowFilter4 = 1.0;
float keyfollowFilter5 = 1.0;
float octaveControlFilter1 = 8.0;
float octaveControlFilter2 = 8.0;
float octaveControlFilter3 = 8.0;
float octaveControlFilter4 = 8.0;
float octaveControlFilter5 = 8.0;
float octaveControlPreNoiseFilter = 1.0;
float offsetNoteKeyfollow = 60.0; // 84 = C6, 72 = C5, 60 = C4
float offsetNoteKeyfollowFilter5 = 40.0; // 84 = C6, 72 = C5, 60 = C4
float offsetNoteKeyfollowNoise = 60.0;  // 84 = C6, 72 = C5, 60 = C4
float offsetNoteKeyfollowPreNoise = 48.0;  // 84 = C6, 72 = C5, 60 = C4
float minPreNoiseNoteNumbr = 60.0;  // 84 = C6, 72 = C5, 60 = C4  4000s noise stops changing below about C4
const uint32_t updatedAudioSystemInterval = 1;  // milliseconds
float freqOscFilterOctaveOffset  = 0.0;    // use 3 to divide FreqOscFilterN by 2^3 to allow modulation to go from -1 to +3/7 
//float fOFOFactor = 1.0; 
float maxFilterFreq = 20000; //12000.0; 
float minFilterFreq = 65.4; // min note number 36 (C2)
float minFilterPreNoiseFreq = 65.4; // 
float FreqPreNoiseVariableFreq = 261.63f; // TODO: match 4000s
float maxDelayDamp = 3000.0; //TODO: find out correct value
float maxDelayLevel = 0.15; //TODO: find out correct value
float gammaDelayLevel = 3.0; //TODO: find out correct value
float gammaDelayFeedback = 1.5; //TODO: find out correct value
float maxTimeNoise = 1000;  // 1000 ms
float TimeNoiseGamma = 4.0;  
float maxNoiseLevel = 0.23;  
float logPotYmidLevelNoise = 0.5f;
float minGamma = 0.1;
float maxGamma = 2.0;
float maxReverbLevel = 0.3f; //0.3;
float maxDenseEarly = 1.0f; //0.8;
bool  porta_step = false;    // round to nearest note for portamento = glissando  // Not really in EWI 4k, but I like it :)
float octaveControlOsc1 = 1.0;
float octaveControlOsc2 = 1.0;
float limitBreathSweepOsc1 = 1.55/(octaveControlOsc1*12);  // 4000s is about 1.55 semitiones 
float limitBreathSweepOsc2 = 1.55/(octaveControlOsc2*12); // 4000s is about 1.55 semitiones  
float maxMixNtcFilter = 1.0;  //0.6; 
//float fPotValue = 0.0; 
//float iPotValue = 0; 
float extraAmpFactor = 2.0f/LevelOscN_HeadRoom;  // 1/LevelOscN_HeadRoom*3.0f
float extraLineInAmpFactor = 1.0;
float mix_lineInLR_gain_0 = volf;
float mix_lineInLR_gain_1 = extraLineInAmpFactor*mix_lineinf;



// current patch nrpn numbers
patch_t current_patch = {
 "Init Patch", 
 {0x40, 0x40, 0x40, 0x40, 0x00, 0x7b, 0x34, 0x00, 0x44, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x7f},
 {0x40, 0x40, 0x40, 0x48, 0x00, 0x7f, 0x44, 0x00, 0x64, 0x11, 0x1e, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x0a},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x0e, 0x00, 0x40, 0x00, 0x00, 0x10, 0x40},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x01, 0x00, 0x40, 0x00, 0x2c, 0x24, 0x4c},
 {0x00, 0x52, 0x0b, 0x40, 0x6c, 0x4d, 0x00, 0x40, 0x4c, 0x50, 0x28, 0x40},
 {0x00, 0x3f, 0x7f, 0x40, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40},
 {0x01, 0x4c, 0x0c},
 {0x00, 0x7f, 0x32},
 {0x02, 0x00, 0x7f, 0x02, 0x02, 0x00, 0x00, 0x01, 0x7f, 0x01},
 {0x7f, 0x7f, 0x40},
 {0x20, 0x3b, 0x6c, 0x48, 0x46, 0x5c, 0x60, 0x0f, 0x7f},
 {0x24, 0x2c, 0x20, 0x44, 0x00},
 {0x7f, 0x6c, 0x55, 0x16, 0x3a}
};  // 154 bytes long

patch_t swap_buffer_patch = {
 "Init Patch", 
 {0x40, 0x40, 0x40, 0x40, 0x00, 0x7b, 0x34, 0x00, 0x44, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x7f},
 {0x40, 0x40, 0x40, 0x48, 0x00, 0x7f, 0x44, 0x00, 0x64, 0x11, 0x1e, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x0a},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x0e, 0x00, 0x40, 0x00, 0x00, 0x10, 0x40},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x01, 0x00, 0x40, 0x00, 0x2c, 0x24, 0x4c},
 {0x00, 0x52, 0x0b, 0x40, 0x6c, 0x4d, 0x00, 0x40, 0x4c, 0x50, 0x28, 0x40},
 {0x00, 0x3f, 0x7f, 0x40, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40},
 {0x01, 0x4c, 0x0c},
 {0x00, 0x7f, 0x32},
 {0x02, 0x00, 0x7f, 0x02, 0x02, 0x00, 0x00, 0x01, 0x7f, 0x01},
 {0x7f, 0x7f, 0x40},
 {0x20, 0x3b, 0x6c, 0x48, 0x46, 0x5c, 0x60, 0x0f, 0x7f},
 {0x24, 0x2c, 0x20, 0x44, 0x00},
 {0x7f, 0x6c, 0x55, 0x16, 0x3a}
};  // 154 bytes long

patch_t copy_buffer_patch = {
 "Init Patch",
 {0x40, 0x40, 0x40, 0x40, 0x00, 0x7b, 0x34, 0x00, 0x44, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x7f},
 {0x40, 0x40, 0x40, 0x48, 0x00, 0x7f, 0x44, 0x00, 0x64, 0x11, 0x1e, 0x40, 0x00, 0x40, 0x00, 0x7f, 0x00, 0x0a},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x0e, 0x00, 0x40, 0x00, 0x00, 0x10, 0x40},
 {0x00, 0x7c, 0x05, 0x40, 0x7f, 0x01, 0x00, 0x40, 0x00, 0x2c, 0x24, 0x4c},
 {0x00, 0x52, 0x0b, 0x40, 0x6c, 0x4d, 0x00, 0x40, 0x4c, 0x50, 0x28, 0x40},
 {0x00, 0x3f, 0x7f, 0x40, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40},
 {0x01, 0x4c, 0x0c},  // spare
 {0x00, 0x7f, 0x32},
 {0x02, 0x00, 0x7f, 0x02, 0x02, 0x00, 0x00, 0x01, 0x7f, 0x01},
 {0x7f, 0x7f, 0x40},
 {0x20, 0x3b, 0x6c, 0x48, 0x46, 0x5c, 0x60, 0x0f, 0x7f},
 {0x24, 0x2c, 0x20, 0x44, 0x00},
 {0x7f, 0x6c, 0x55, 0x16, 0x3a}
};  // 154 bytes long

fx_t global_buffer_fx = {
 "Global Fx",   // char fx_string[32];
 {0x20, 0x3b, 0x6c, 0x48, 0x46, 0x5c, 0x60, 0x0f, 0x7f},  // uint8_t nrpn_msb_chorus[NRPN_MSB_CHORUS_LENGTH];  // 9 
 {0x24, 0x2c, 0x20, 0x44, 0x00}, // uint8_t nrpn_msb_delay[NRPN_MSB_DELAY_LENGTH];    // 5 
 {0x7f, 0x6c, 0x55, 0x16, 0x3a}, // uint8_t nrpn_msb_reverb[NRPN_MSB_REVERB_LENGTH];   // 5
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01} // uint8_t nrpn_msb_common1[NRPN_MSB_COMMON1_LENGTH] //10
};  // 61 bytes long

fx_t copy_buffer_fx = {
 "Copied Fx",   // char fx_string[32];
 {0x20, 0x3b, 0x6c, 0x48, 0x46, 0x5c, 0x60, 0x0f, 0x7f},  // uint8_t nrpn_msb_chorus[NRPN_MSB_CHORUS_LENGTH];  // 9 
 {0x24, 0x2c, 0x20, 0x44, 0x00}, // uint8_t nrpn_msb_delay[NRPN_MSB_DELAY_LENGTH];    // 5 
 {0x7f, 0x6c, 0x55, 0x16, 0x3a}, // uint8_t nrpn_msb_reverb[NRPN_MSB_REVERB_LENGTH];   // 5
 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01} // uint8_t nrpn_msb_common1[NRPN_MSB_COMMON1_LENGTH] //10
};  // 61 bytes long

// TODO: fill this in 
ext_t extensions_buffer_ext = {
    "Extensions", 
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //uint8_t nrpn_msb_compressor[NRPN_MSB_COMP_LENGTH];  // 7
};

const String alphaNumString(F("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+-`'|/ "));


const uint32_t eepromUpdateInterval = 60000;  // milliseconds (120,000 = 2min)
uint32_t eepromPreviousMillis = 0;
uint32_t eepromCurrentMillis = 0;
bool programChangeFlag = false; // new programChange is happening
int programChangeData = 0; // value from midi_ho
bool updateEpromFlag = false;
int current_patchNumber = 0;
int sourcePatchNumber = 0;
int paste_patchNumber = 0;
int swap_patchNumber = 0;
int eeprom_patchNumber = 0;
int paste_FxNumber = 0;
DMAMEM patch_t loadedPatches[NUMBER_OF_PATCHES];
bool patchLoaded[NUMBER_OF_PATCHES] = {0};
bool globalFxLoaded = 0;
File dataFile;
uint8_t sysexPresetSendBuffer[206] = {    // EWI_SYSEX_PRESET_DUMP_LEN
    0xF0, 0x47, 0x64, 0x00, 0x20, 0x00, 0x45, 0x34, 0x6B, 0x09, 0x26, 0x06, 0x53, 0x75, 0x6D, 0x6F,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x40, 0x12, 0x00, 0x40,
    0x40, 0x40, 0x40, 0x00, 0x4E, 0x00, 0x47, 0x27, 0x18, 0x0F, 0x2B, 0x08, 0x41, 0x00, 0x3F, 0x00,
    0x7F, 0x41, 0x12, 0x00, 0x41, 0x47, 0x40, 0x40, 0x00, 0x00, 0x5B, 0x00, 0x59, 0x27, 0x1E, 0x3F,
    0x18, 0x40, 0x00, 0x4D, 0x27, 0x7F, 0x48, 0x0C, 0x00, 0x00, 0x69, 0x16, 0x4A, 0x35, 0x68, 0x00,
    0x40, 0x7D, 0x3C, 0x57, 0x48, 0x49, 0x0C, 0x00, 0x00, 0x7C, 0x0B, 0x40, 0x00, 0x01, 0x00, 0x40,
    0x00, 0x40, 0x00, 0x40, 0x4A, 0x0C, 0x00, 0x00, 0x6D, 0x0B, 0x49, 0x6C, 0x4D, 0x00, 0x40, 0x4C,
    0x40, 0x0F, 0x40, 0x4B, 0x0C, 0x00, 0x04, 0x24, 0x0B, 0x40, 0x00, 0x01, 0x00, 0x40, 0x00, 0x40,
    0x00, 0x40, 0x4F, 0x03, 0x00, 0x01, 0x4C, 0x0C, 0x50, 0x03, 0x00, 0x4C, 0x46, 0x00, 0x51, 0x0A,
    0x00, 0x01, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7F, 0x00, 0x58, 0x03, 0x00, 0x42, 0x17,
    0x17, 0x70, 0x09, 0x00, 0x03, 0x3B, 0x7C, 0x11, 0x3C, 0x7F, 0x46, 0x32, 0x7F, 0x71, 0x05, 0x00,
    0x2A, 0x2C, 0x12, 0x45, 0x7F, 0x72, 0x05, 0x00, 0x7F, 0x68, 0x55, 0x18, 0x3A, 0xF7
};

//typedef enum { LP, HP, BP, NTC, THRU } filterMode_t; // in head file now

// patch synth variables
float OctOsc1 = 0;      //64,0,62,66,
float SemiOsc1 = 0.0;     //64,1,52,76, -12 to +12
float FineOsc1 = 0.0;  	//64,2,14,114, -50cents/100 to +50cents/100
float BeatOsc1 = 0.0;  	//64,3,0,127, 0 to 1.0
float BeatMax = 5.33f;  	
float SawOsc1 = 0.0;  	//64,5,0,127, 0 to 1.0
float logPotYmidWaveN = 0.50f;
float TriOsc1 = 1.0;  	//64,6,0,127, 0 to 1.0
float PulseOsc1 = 0.0;  	//64,7,0,127, 0 to 1.0
float PwOffsetOsc1 = 0.0;  	//64,8,0,127, 0 to 1.0
float PwOsc1 = PwOffsetOsc1;  	//64,8,0,127, 0 to 1.0
float PwmFreqOsc1 = 0.0*maxPwmLfoFreq;   	//64,9,0,127,   // 0.0 to 1.0 * maxPwmLfoFreq 
float PwmDepthOsc1 = 0.0;  	//64,10,0,127,
float SweepDepthOsc1 = 0.0;  	//64,11,0,127,
float SweepTimeOsc1 = 0.0;  	//64,12,0,127,
float BreathDepthOsc1 = 0.0;  	//64,13,0,127, <64 start flat, >64 start sharp, -1.0 to +1.0
float BreathAttainOsc1 = 0.0;  	//64,14,0,127,
float BreathCurveOsc1 = 0.7;  	//64,15,0,127,
CurveLines BreathOscCurveLines1 = {0.0f, 1.0f, 1.0f};
float BreathThreshOsc1 = 0;  	//64,16,0,127,
float LevelOsc1 = 0;  		//64,17,0,127,
float LimiterAmount = 0.0; // 0 <= LimiterAmout <= 0.5, where 0 is linear
//float LevelOscN_HeadRoom = 1.0f/2.75; //1.0f/2.2f;  //extraAmpFactor //64,17,0,127,
float LevelOscN_HeadRoom = 1.0f/1.7; //1.0f/2.2f;  //extraAmpFactor //64,17,0,127,
float logPotYmidLevelOscN = 0.50f; // 0.5 is linear, 0.8 means at 0.5 in you get 0.8 out
float Amp_HeadRoom = 1.0f;  		//64,17,0,127,
float OctOsc2 = 0;  	//65,0,62,66,
float SemiOsc2 = 0;  	//65,1,52,76,
float FineOsc2 = 0;  	//65,2,14,114,
float BeatOsc2 = 0;  	//65,3,0,127,
float SawOsc2 = 1.0;  	//65,5,0,127,
float TriOsc2 = 0;  	//65,6,0,127,
float PulseOsc2 = 0.0;  	//65,7,0,127,
float PwOffsetOsc2 = 0.0;  	//64,8,0,127, 0 to 1.0
float PwOsc2 = PwOffsetOsc2;  	//65,8,0,127,
float PwmFreqOsc2 = 0.0;  	//65,9,0,127,
float PwmDepthOsc2 = 0.0;  	//65,10,0,127,
float SweepDepthOsc2 = 0.0;  	//65,11,0,127,
float SweepTimeOsc2 = 0.0;  	//65,12,0,127,
float BreathDepthOsc2 = 0.0;  	//65,13,0,127,
float BreathAttainOsc2 = 0.0;  	//65,14,0,127,
float BreathCurveOsc2 = 1.2;  	//65,15,0,127, +63, -64, so 1+(x-64)*DIV64
CurveLines BreathOscCurveLines2 = {0.0f, 1.0f, 1.0f};
float BreathThreshOsc2 = 0.0;  	//65,16,0,127,
float LevelOsc2 = 0;  	//65,17,0,127,
filterMode_t ModeOscFilter1 = LP;  	//72,0,0,4,//LP HP BP NTC OFF
float FreqOscFilter1  = 1046.5;    // C6  	//72,1,36,124,//Midi Note 36 to 124
float FreqOscFilter1BModFactor  = 1.0; // Freq slider 36 to 124 (when slider = 96 (c7), BModFactor is 1.0) 
float QFactorOscFilter1 = 0.707;  	//72,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowOscFilter1 = 12;  	//72,3,52,88,// -12 to +24 num semi tones
float BreathModOscFilter1 = 0;  	//72,4,0,127,
float LfoFreqOscFilter1 = 0;  	//72,5,0,127,
float LfoDepthOscFilter1 = 0;  	//72,6,0,127,
float LfoBreathOscFilter1 = 0;  	//72,7,0,127,
float LfoThreshOscFilter1 = 0;  	//72,8,0,127,
float SweepDepthOscFilter1 = 0;  	//72,9,0,127,
float SweepTimeOscFilter1 = 0;  	//72,10,0,127,
float BreathCurveOscFilter1 = 1.0;  	//72,11,0,127,  TODO: hook this up
CurveLines BreathOscFiltCurveLines1 = {0.0f, 1.0f, 1.0f};
filterMode_t ModeOscFilter2 = LP;  	//73,0,0,4,//LP HP BP NTC OFF
filterMode_t ModeOscFilter2_stored = LP;  	//73,0,0,4,//LP HP BP NTC OFF
float FreqOscFilter2 = 1046.5;    // C6  	//73,1,36,124,//Midi Note 36 to 124
float FreqOscFilter2BModFactor  = 1.0; // Note number of Freq slider 36 to 124
float QFactorOscFilter2 = 0.707;  	//73,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowOscFilter2 = 12;  	//73,3,52,88,// -12 to +24 num semi tones
float BreathModOscFilter2 = 0;  	//73,4,0,127,
float LfoFreqOscFilter2 = 0;  	//73,5,0,127,
float LfoDepthOscFilter2 = 0;  	//73,6,0,127,
float LfoBreathOscFilter2 = 0;  	//73,7,0,127,
float LfoThreshOscFilter2 = 0;  	//73,8,0,127,
float SweepDepthOscFilter2 = 0;  	//73,9,0,127,
float SweepTimeOscFilter2 = 0;  	//73,10,0,127,
float BreathCurveOscFilter2 = 1.0;  	//73,11,0,127, TODO: hook this up
CurveLines BreathOscFiltCurveLines2 = {0.0f, 1.0f, 1.0f};
filterMode_t ModeNoiseFilter3 = LP;  	//74,0,0,4,//LP HP BP NTC OFF
float FreqNoiseFilter3 = 1046.5;    // C6  	//74,1,36,124,//Midi Note 36 to 124
float FreqNoiseFilter3BModFactor  = 1.0; // Note number of Freq slider 36 to 124
float QFactorNoiseFilter3 = 1.0;  	//74,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowNoiseFilter3 = 12;  	//74,3,52,88,// -12 to +24 num semi tones
float BreathModNoiseFilter3 = 0;  	//74,4,0,127,
float LfoFreqNoiseFilter3 = 0;  	//74,5,0,127,
float LfoDepthNoiseFilter3 = 0;  	//74,6,0,127,
float LfoBreathNoiseFilter3 = 0;  	//74,7,0,127,
float LfoThreshNoiseFilter3 = 0;  	//74,8,0,127,
float SweepDepthNoiseFilter3 = 0;  	//74,9,0,127,
float SweepTimeNoiseFilter3 = 0;  	//74,10,0,127,
float BreathCurveNoiseFilter3 = 1.0;  	//74,11,0,127, TODO: hook this up
CurveLines BreathNoiseFiltCurveLines3 = {0.0f, 1.0f, 1.0f};
filterMode_t ModeNoiseFilter4 = LP;  	//75,0,0,4,//LP HP BP NTC OFF
filterMode_t ModeNoiseFilter4_stored = LP;  	//73,0,0,4,//LP HP BP NTC OFF
float FreqNoiseFilter4 = 1046.5;    // C6  	//75,1,36,124,//Midi Note 36 to 124
float FreqNoiseFilter4BModFactor  = 1.0; // Note number of Freq slider 36 to 124
float QFactorNoiseFilter4 = 2.5;  	//75,2,5,127,"// 5=0.5, 127=12.7"
float QFactorFilter5 = .707;  	//75,2,5,127,"// 5=0.5, 127=12.7"
float KeyFollowNoiseFilter4 = 12;  	//75,3,52,88,// -12 to +24 num semi tones
float KeyFollowFilter5 = 1;  	//75,3,52,88,// -12 to +24 num semi tones
float BreathModNoiseFilter4 = 0;  	//75,4,0,127,
float LfoFreqNoiseFilter4 = 0;  	//75,5,0,127,
float LfoDepthNoiseFilter4 = 0;  	//75,6,0,127,
float LfoBreathNoiseFilter4 = 0;  	//75,7,0,127,
float LfoThreshNoiseFilter4 = 0;  	//75,8,0,127,
float SweepDepthNoiseFilter4 = 0;  	//75,9,0,127,
float SweepTimeNoiseFilter4 = 0;  	//75,10,0,127,
float BreathCurveNoiseFilter4 = 1.0;  	//75,11,0,127, TODO: hook this up
CurveLines BreathNoiseFiltCurveLines4 = {0.0f, 1.0f, 1.0f};
float KeyFollowPreNoiseFilter = 12;  // TODO: match 4000s
float keyfollowFilterPreNoise = 1.0; 
float FreqPreNoiseFilter = 4000.0f; // TODO: match 4000s
float NoiseTime = 0.0;  	//80,0,0,127,
float TimeNoiseAmp = 1.0;  	//80,0,0,127,
float NoiseBreathCurve = 1.0;  	//80,1,0,127,
CurveLines NoiseBreathCurveLines = {0.0f, 1.0f, 1.0f};
float NoiseLevel = maxNoiseLevel;  	//80,2,0,127,
float BendRange = 2.0;  	//81,0,0,12,// num semitones
bool BendStep = false;  	//81,1,0,1,//0=off 1=on
float VibratoPitch = 0;  	//81,2,0,127,
bool LinkOscFilters = true;  	//81,3,1,2,//1=Link on 2= Link off  TODO: hook this up
bool LinkNoiseFilters = true;  	//81,4,1,2,                         TODO: hook this up
uint8_t Formant = 0;  	        //81,5,0,2,//0=off 1=W.Wind 2=String
bool XFade = true;  	        //81,6,0,1,//0=off 1=on
bool KeyTriggerSingle = 1;  	    //81,7,0,1,//0=Multi 1=Single
float s81_8 = 127;
bool ChorusOn = 1;
float VibratoAmp = 0;  	    //88,0,0,127,
float AmpLevel = 0.5;  	    //88,1,0,127,
float mix_Amp_gain_0 =  AmpLevel*Amp_HeadRoom;
float mix_Amp_gain_1 =  1.0f;
float OctButtonLevel = 0;  	    //88,2,0,127,
float EffectsChorusDelay1 = 0;  //112,0,0,127,
float EffectsChorusMod1 = 0;  	//112,1,0,127,
float EffectsChorusWet1 = 0;  	//112,2,0,127,
float EffectsChorusDelay2 = 0;  //112,3,0,127,
float EffectsChorusMod2 = 0;  	//112,4,0,127,
float EffectsChorusWet2 = 0;  	//112,5,0,127,
float EffectsChorusFeedback = 0;//112,6,0,127,
float EffectsChorusLfoFreq = 0; //112,7,0,127,
float EffectsChorusDryLevel = 0;//112,8,0,127,
float EffectsDelayTimeL = 0;  	//113,0,0,127,
float EffectsDelayTimeR= 0;  	//113,0,0,127,
float EffectsDelayFeedback = 0; //113,1,0,127,
float EffectsDelayDamp = 10000;  	//113,2,0,127,
float EffectsDelayLevel = 0;  	//113,3,0,127,
float EffectsDelayPong = 0.0;  	//113,3,0,127,
float EffectsReverbSpare = 1.0;  	//114,1,0,127,
float EffectsReverbLevel = 0;  	//114,1,0,127,
float EffectsReverbDenseEarly = 0;  	//114,2,0,127,
float EffectsReverbTime = 0;  	//114,3,10,50,//1.0 to 5.0 sec
float EffectsReverbDamp = 0;  	//114,4,54,74,//-10 to +10
float EffectsChorusFBHeadroom = 0.8; // to reduce levels of chorus fb mixer to keep from clipping
float EffectsChorusDryHeadroom = 0.1; //0.8; // to reduce levels of chorus dry mixer to keep from clipping

// Number of samples in each delay line
// AUDIO_BLOCK_SAMPLES = 128
// 4k GUI says 127 mS max delay = 44 blocks at 44.1khz but flange cuts delay in half
// 44.1 samples/ms, so 88.2 samples/ms when using flange
// #define FLANGE_DELAY_LENGTH (176*AUDIO_BLOCK_SAMPLES)
// Allocate the delay lines for left and right channels
short delayline_flange1[FLANGE_DELAY_LENGTH];
short delayline_flange2[FLANGE_DELAY_LENGTH];
short delayline_flange3[FLANGE_DELAY_LENGTH];

filterMode_t modeFilter1_old = THRU;
filterMode_t modeFilter2_old = THRU;
filterMode_t modeFilter3_old = THRU;
filterMode_t modeFilter4_old = THRU;

OledMenu myMenu;

