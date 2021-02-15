#define TEST

#include "constants.h"

enum STATE {ENABLE, DISABLE};

enum COMPONENT_TYPE {
  OUT_OF_ORDER,
  ARDUINO_ANALOG, ARDUINO_PIN_PULL_DOWN, ARDUINO_PIN_PULL_UP, ARDUINO_PIN_INPUT_PULLUP,
  CD4051_ANALOG, CD4051_PULL_DOWN, CD4051_PULL_UP
};

enum MIDI_MESSAGE_TYPE {
  CC = 0xB0 , NOTE_ON = 0x90 , PITCH_BEND = 0xE0
};

/*********************************************************************/

struct B {
  const byte pin;
  const COMPONENT_TYPE ct;
  const byte midi_channel;
  const MIDI_MESSAGE_TYPE mess_type;
  const byte midi_note;
  int old_value;//1023 max > 255
  int value;//1023 max > 255
  unsigned long last_debounce_time;
  B(): ct{OUT_OF_ORDER} {}
  B(COMPONENT_TYPE b, byte c, MIDI_MESSAGE_TYPE d, byte e)
    : ct{b}, midi_channel{c}, mess_type{d}, midi_note{e} {
    old_value = value = last_debounce_time = 0;
  }
  B(byte a, COMPONENT_TYPE b, byte c, MIDI_MESSAGE_TYPE d, byte e)
    : pin{a}, ct{b}, midi_channel{c}, mess_type{d}, midi_note{e} {
    old_value = value = last_debounce_time = 0;
  }
};

B B_ARRAY[] = {
  {2, ARDUINO_PIN_INPUT_PULLUP, CHAN8, NOTE_ON, CONTROL1},
  {A1, ARDUINO_ANALOG, CHAN8, NOTE_ON, CONTROL2},
  {A2, ARDUINO_ANALOG, CHAN8, NOTE_ON, CONTROL3},
};

const int NB_B = sizeof(B_ARRAY) / sizeof(B);

/*********************************************************************/

const byte NCD = 2;

struct D {
  byte read_pin;
  byte out_pins[3];
  byte inhib[NCD];
  B b[8 * NCD];
};

D sd = {
  A0,
  {8, 9, 10},
  {6, 7},
  {
    {CD4051_ANALOG, CHAN8, NOTE_ON, CONTROL11},
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    B(),
    {CD4051_PULL_DOWN, CHAN8, NOTE_ON, CONTROL12},
    {CD4051_ANALOG, CHAN8, NOTE_ON, CONTROL13},
  },
};

/*********************************************************************/

void setup() {
  Serial.begin(115200);
  //  sd_print();
  sd_init();
  bu_init();
}

void loop () {
//  unsigned long t2 = millis();
  sd_update_state();
//  Serial.println(millis()-t2);
//  delay(200);
  bu_manage();
}
