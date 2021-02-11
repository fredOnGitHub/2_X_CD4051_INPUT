#include "constants.h"
#define TEST

enum STATE {ENABLE, DISABLE};
enum COMPONENT_TYPE {
  OUT_OF_ORDER,
  ARDUINO_ANALOG, ARDUINO_PIN_PULL_DOWN, ARDUINO_PIN_PULL_UP, ARDUINO_PIN_INPUT_PULLUP,
  CD4051_ANALOG, CD4051_PULL_DOWN, CD4051_PULL_UP
};
enum MIDI_MESSAGE_TYPE {
  CC = 0xB0 , NOTE_ON = 0x90 , PITCH_BEND = 0xE0
};

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

void setup() {
  Serial.begin(115200);
  //  sd_print();
  sd_init();
  bu_init();
}

void loop () {
  sd_update_state();
  bu_manage();
}

void b_manage(B*x, int reading) {
  switch (x->ct) {
    case ARDUINO_ANALOG:
    case CD4051_ANALOG: {//https://rules.sonarsource.com/c/RSPEC-1151
        //Serial.println("--ANALOG");
        x->value = reading;
        int tmp = (x->old_value - x->value);
        if (tmp >= 8 || tmp <= -8) {
          x->old_value = x->value;
#if defined (TEST)
          print_type(x, reading);
#else
          Serial.write(x->mess_type + x->midi_channel);//10 bits => (0..1023)/8 == (0..127)
          Serial.write(x->midi_note);//key note
          Serial.write(x->value / 8);
#endif
        }
        break;
      }
    case OUT_OF_ORDER:
      break;
    default:
      {
        //Serial.println("--AUTRES");
        if (reading != x->old_value) {
          x->last_debounce_time = millis();
        }
        if ((millis() - x->last_debounce_time) > 5) {
          if (reading != x->value) {
            x->value = reading;
            if (
              (x->ct == ARDUINO_PIN_PULL_DOWN && x->value > LOW)
              || (x->ct == ARDUINO_PIN_PULL_UP && x->value == LOW)
              || (x->ct == ARDUINO_PIN_INPUT_PULLUP && x->value == LOW)
              || (x->ct == CD4051_PULL_DOWN && x->value > LOW)
              || (x->ct == CD4051_PULL_UP && x->value == LOW)
            ) {
#if defined (TEST)
              print_type(x, reading);
#else
              Serial.write(x->mess_type + x->midi_channel);
              Serial.write(x->midi_note);//key note
              Serial.write(0x40); //a velocity
#endif
            }
          }
        }
        x->old_value = reading;
        break;
      }
  }
}

void print_with_pin(const char *c, B *x, const int v) {
  Serial.print(c);
  Serial.print("  ");
  Serial.print(x->pin, DEC);
  Serial.print("  ");
  Serial.println(v);
}
void print1(const char *c, const int v) {
  Serial.print(c);
  Serial.print("  ");
  Serial.println(v);
}
void print_type(B *x, int v) {
  switch (x->ct) {
    case OUT_OF_ORDER:
      Serial.println("OUT_OF_ORDER");
      break;
    case ARDUINO_ANALOG: {
        print_with_pin("ARDUINO_ANALOG", x, v);
        break;
      }
    case ARDUINO_PIN_PULL_DOWN: {
        print_with_pin("ARDUINO_PIN_PULL_DOWN", x, v);
        break;
      }
    case ARDUINO_PIN_PULL_UP: {
        print_with_pin("ARDUINO_PIN_PULL_UP", x, v);
        break;
      }
    case ARDUINO_PIN_INPUT_PULLUP: {
        print_with_pin("ARDUINO_PIN_INPUT_PULLUP", x, v);
        break;
      }
    case CD4051_ANALOG:
    case CD4051_PULL_DOWN:
    case CD4051_PULL_UP:
      sd_print_state();
      break;
    default:
      break;
  }
}
