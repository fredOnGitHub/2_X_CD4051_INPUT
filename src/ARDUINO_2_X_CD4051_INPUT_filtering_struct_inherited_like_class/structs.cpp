#include "structs.hpp"

STR::STR(byte a, byte b, byte c): midi_channel{a}, mess_type{b}, midi_note{c} {
#if defined (TEST)
  sc("STR(...)");
#endif
  old_value = 0;
  value = 0;
}
void STR::pr() {
  sc2("midi_channel : "); si(midi_channel);
  sc2("mess_type : "); si(mess_type);
  sc2("midi_note : "); si(midi_note);
}

ARDUINO_B::ARDUINO_B(byte d, BUTTON_TYPE e, byte a, byte b, byte c):
  pin{d}, button_type{e}, STR{a, b, c} {
#if defined (TEST)
  sc("ARDUINO_B(...)");
#endif
  last_debounce_time = 0;
}

void ARDUINO_B::pr() {
  sc2("pin : "); si(pin);
  sc2("button_type ! "); si(button_type);
  STR::pr();
}

/*****************************************************************************/

ARDUINO_P::ARDUINO_P(byte d, byte a, byte b, byte c):
  pin{d}, STR{a, b, c} {
#if defined (TEST)
  sc("ARDUINO_P(...)");
#endif
}

void ARDUINO_P::pr() {
  sc2("pin : "); si(pin);
  STR::pr();
}

/*****************************************************************************/

CD4051_B::CD4051_B(int d, BUTTON_TYPE e, byte a, byte b, byte c):
  pos{d}, button_type{e}, STR{a, b, c} {
#if defined (TEST)
  sc("CD4051_B(...)");
#endif
  last_debounce_time = 0;
}

void CD4051_B::pr() {
  sc2("pos : "); si(pos);
  sc2("button_type : "); si(button_type);
  STR::pr();
}

/*****************************************************************************/

CD4051_P::CD4051_P(int d, byte a, byte b, byte c):
  pos{d}, STR{a, b, c} {
#if defined (TEST)
  sc("CD4051_P(...)");
#endif
}

void CD4051_P::pr() {
  sc2("pos : "); si(pos);
  STR::pr();
}

/*****************************************************************************/

AB::AB(ARDUINO_B *a, int b): a{a}, l{b} {
#if defined (TEST)
  sc("AB(...)");
#endif
}
AP::AP(ARDUINO_P *a, int b): a{a}, l{b} {
#if defined (TEST)
  sc("AP(...)");
#endif
}

CB::CB(CD4051_B *a, int b): a{a}, l{b} {
#if defined (TEST)
  sc("CB(...)");
#endif
}
CP::CP(CD4051_P *a, int b): a{a}, l{b} {
#if defined (TEST)
  sc("CP(...)");
#endif
}

/*****************************************************************************/

void AB::init() {
#if defined (TEST)
  Serial.println("ab init ");
#endif
  for (int i = 0; i < l; i++) {
    //      Serial.println(i,DEC);
    ARDUINO_B *x = &a[i];
    switch (x->button_type) {
      case ARDUINO_PIN_PULL_DOWN: {
          x->value = x->old_value = 0;
          pinMode(x->pin, INPUT);
          break;
        }
      case ARDUINO_PIN_PULL_UP: {
          x->value = x->old_value = 1;
          pinMode(x->pin, INPUT);
          break;
        }
      case ARDUINO_PIN_INPUT_PULLUP: {
          x->value = x->old_value = 1;
          pinMode(x->pin, INPUT_PULLUP);
          break;
        }
      default:
        break;
    }
  }
}
void AB::action() {
  for (int i = 0; i < l; i++) {
    ARDUINO_B *x = &a[i];
    int r = digitalRead(x->pin);
    if (r != x->old_value) {
      x->last_debounce_time = millis();
    }
    if ((millis() - x->last_debounce_time) > debounce_time) {
      if (x->value != r) {
        x->value = r;
        if (
          (x->button_type == ARDUINO_PIN_PULL_DOWN && x->value > LOW)
          || (x->button_type == ARDUINO_PIN_PULL_UP && x->value == LOW)
          || (x->button_type == ARDUINO_PIN_INPUT_PULLUP && x->value == LOW)
        ) {
#if defined (TEST)
          Serial.print("ab action ");
          Serial.println(r, DEC);
#else
          Serial.write(x->mess_type + x->midi_channel);
          Serial.write(x->midi_note);//key note
          Serial.write(0x40); //a velocity
#endif
        }
      }
    }
    x->old_value = r;
  }
}

void AP::action() {
  for (int i = 0; i < l; i++) {
    ARDUINO_P *x = &a[i];
    int r = analogRead(x->pin);
    x->value = r;
    int tmp = (x->old_value - x->value);
    if (tmp >= 8 || tmp <= -8) {
      x->old_value = r;
#if defined (TEST)
      Serial.print("ap action ");
      Serial.print(r, DEC);
      Serial.print("  pin : ");
      Serial.println(x->pin, DEC);
#else
      Serial.write(x->mess_type + x->midi_channel);//10 bits => (0..1023)/8 == (0..127)
      Serial.write(x->midi_note);//key note
      Serial.write(x->value / 8);
#endif
    }
  }
}

void PILOT_CD4051::init() {
  pinMode(out_pins[0], OUTPUT);
  pinMode(out_pins[1], OUTPUT);
  pinMode(out_pins[2], OUTPUT);
  for (byte k = 0; k < NINHIBIT; k++) {
    pinMode (inhib[k], OUTPUT);
  }
}
void PILOT_CD4051::write_all_inh_at_state(STATE b) {
  for (byte k = 0; k < NINHIBIT; k++) {
    digitalWrite (inhib[k], b);
  }
}
int PILOT_CD4051::readIC(int pos2) {
  byte a, b, c;
  int n = 0;
  while (pos2 > 7) {
    pos2 -= 8;
    n++;
  }
  //      Serial.print("pos2 "); Serial.println(pos2, DEC);
  //      Serial.print("n "); Serial.println(n, DEC);
  write_all_inh_at_state(DISABLE);
  digitalWrite (inhib[n], ENABLE);
  a = (1 && (pos2 & B00000001));
  b = (1 && (pos2 & B00000010));
  c = (1 && (pos2 & B00000100));
  digitalWrite(out_pins[0], a);
  digitalWrite(out_pins[1], b);
  digitalWrite(out_pins[2], c);
  return analogRead(read_pin);
}
void PILOT_CD4051::the_CD4051_B() {
  int pos2, r, i;
  CD4051_B *x;
  for (i = 0; i < cb->l; i++) {
    x = &cb->a[i];
    r = readIC(x->pos);
    //      delay(200);
    //      Serial.print("r "); Serial.println(r, DEC);
    //      Serial.print("x->value "); Serial.println(x->value, DEC);
    //      Serial.print("x->old_value "); Serial.println(x->old_value, DEC);

    if (r != x->old_value) {
      x->last_debounce_time = millis();
    }
    if ((millis() - x->last_debounce_time) > debounce_time) {
      if (x->value != r) {
        x->value = r;
        if (
          (x->button_type == CD4051_PULL_DOWN && x->value > LOW)
          || (x->button_type == CD4051_PULL_UP && x->value == LOW)
        ) {
#if defined (TEST)
          Serial.print("the_CD4051_B ");
          Serial.print(r, DEC);
          Serial.print("  pos : ");
          Serial.println(x->pos, DEC);
          //            print21("action_CD4051_BU  ", x->value, pos);
#else
          Serial.write(x->mess_type + x->midi_channel);
          Serial.write(x->midi_note);//key note
          Serial.write(0x40); //a velocity
#endif
        }
      }
    }
    x->old_value = r;
  }
}
void PILOT_CD4051::the_CD4051_P() {
  CD4051_P *x;
  int r, i;
  for (i = 0; i < cp->l; i++) {
    x = &cp->a[i];
    r = readIC(x->pos);
    x->value = r;

    //      delay(200);
    //      Serial.print("r "); Serial.println(r, DEC);
    //      Serial.print("x->value "); Serial.println(x->value, DEC);
    //      Serial.print("x->old_value "); Serial.println(x->old_value, DEC);

    int tmp = (x->old_value - x->value);
    if (tmp >= 8 || tmp <= -8) {
      x->old_value = r;
#if defined (TEST)
      Serial.print("the_CD4051_P ");
      Serial.print(r, DEC);
      Serial.print("  pos : ");
      Serial.println(x->pos, DEC);
#else
      Serial.write(x->mess_type + x->midi_channel);//10 bits => (0..1023)/8 == (0..127)
      Serial.write(x->midi_note);//key note
      Serial.write(x->value / 8);
#endif
    }
  }
}

void PILOT_CD4051::action() {
  the_CD4051_B();
  the_CD4051_P();
}
