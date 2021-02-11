

B B_ARRAY[] = {
  {2, ARDUINO_PIN_INPUT_PULLUP, CHAN8, NOTE_ON, CONTROL1},
  {A1, ARDUINO_ANALOG, CHAN8, NOTE_ON, CONTROL2},
  {A2, ARDUINO_ANALOG, CHAN8, NOTE_ON, CONTROL3},
};

const int NB_B = sizeof(B_ARRAY) / sizeof(B);

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
