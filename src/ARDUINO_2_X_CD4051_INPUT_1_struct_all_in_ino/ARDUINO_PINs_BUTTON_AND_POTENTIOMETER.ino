
void bu_init() {
  B * x;
  for (int i = 0; i < NB_B; i++) {
    x = B_ARRAY + i;
    x->last_debounce_time = 0;
    switch ( x->ct) {
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

void bu_manage()//modify
{
  B * x;
  for (int i = 0; i < NB_B; i++) {
    x = B_ARRAY + i;
    switch ( x->ct) {
      case ARDUINO_ANALOG:
        b_manage(x, analogRead(x->pin));//!!analogRead
        break;
      case ARDUINO_PIN_PULL_DOWN:
      case ARDUINO_PIN_PULL_UP:
      case ARDUINO_PIN_INPUT_PULLUP:
        b_manage(x, digitalRead(x->pin));
        break;
      default:
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
