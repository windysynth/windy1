/*
  Windy1 USB Host Synth for Wind controllers
       By 2021 Tim King

 * 
 * Name the USB iteration.
 * 
 */

#include "usb_names.h"

#define MIDI_NAME   {'W', 'I', 'N', 'D', 'Y', '1'}
#define MIDI_NAME_LEN  6

struct usb_string_descriptor_struct usb_string_product_name = {
        2 + MIDI_NAME_LEN * 2,
        3,
        MIDI_NAME
};
