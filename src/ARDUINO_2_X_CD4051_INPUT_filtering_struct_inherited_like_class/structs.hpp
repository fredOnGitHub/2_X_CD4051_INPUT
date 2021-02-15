#include <Arduino.h>

//#define TEST

const byte NINHIBIT = 2;
const unsigned long debounce_time  = 5;

inline void sc(const char*c) {
  Serial.println(c);
}
inline void sc2(const char*c) {
  Serial.print(c);
}
inline void si(int c) {
  Serial.println((int)c, DEC);
}

//inline void sr_midi(byte a, byte b, byte c) {
//  Serial.write(a);//mess type + midi channel
//  Serial.write(b);//key note
//  Serial.write(c); //velocity
//}

enum STATE {ENABLE, DISABLE};

enum BUTTON_TYPE {
  ARDUINO_PIN_PULL_DOWN, ARDUINO_PIN_PULL_UP, ARDUINO_PIN_INPUT_PULLUP,
  CD4051_PULL_DOWN, CD4051_PULL_UP
};

enum MIDI_MESSAGE_TYPE {
  NOTE_ON = 0x90/*144*/, CC = 0xB0/*176*/, PITCH_BEND = 0xE0/*224*/
};

enum COMPONENT_TYPE {
  OUT_OF_ORDER,
  CD4051_BU, CD4051_PO,
};

/**************************************/

struct STR {
    STR(byte, byte, byte);
    byte midi_channel;
    byte mess_type;
    byte midi_note;
    int old_value;//0..1023 max > 255
    int value;//0..1023 max > 255
    void pr();
};

/**************************************/

struct ARDUINO_B: STR {
    ARDUINO_B(byte, BUTTON_TYPE, byte, byte, byte);
    BUTTON_TYPE button_type;
    byte pin;
    unsigned long last_debounce_time;
    void pr();
};

struct ARDUINO_P: STR {
    ARDUINO_P(byte, byte, byte, byte);
    byte pin;
    void pr();
};

/**************************************/

struct CD4051_B: STR {
    CD4051_B(int, BUTTON_TYPE, byte, byte, byte);
    int pos;
    BUTTON_TYPE button_type;
    unsigned long last_debounce_time;
    void pr();
};

struct CD4051_P: STR {
    CD4051_P(int, byte, byte, byte);
    int pos;
    void pr();
};
/**************************************/

struct AB {
    ARDUINO_B *a;
    int l;
    AB(ARDUINO_B *, int);
    void init();
    void action();
};

struct AP {
    ARDUINO_P *a;
    int l;
    AP( ARDUINO_P *, int);
    void action();
};

/**************************************/

struct CB {
    CB( CD4051_B *, int);
    CD4051_B *a;
    int l;
};

struct CP{
    CP( CD4051_P *, int);
    CD4051_P *a;
    int l;
};

/**************************************/

struct PILOT_CD4051 {
    byte read_pin;
    byte out_pins[3];
    byte inhib[NINHIBIT];
    CB *cb;
    CP *cp;
    void init();
        void write_all_inh_at_state(STATE);
        int readIC(int);
        void the_CD4051_B();
        void the_CD4051_P();
        void action();
};
