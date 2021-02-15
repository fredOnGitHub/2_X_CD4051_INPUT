void sd_print() {
  for (int d = 0; d < 8 * NCD; d++) {
    Serial.println("----------------------------");
    Serial.println(sd.b[d].ct, DEC);
    Serial.println(sd.b[d].midi_channel, DEC);
    Serial.println(sd.b[d].mess_type, DEC);
    Serial.println(sd.b[d].midi_note, DEC);
    Serial.println(sd.b[d].old_value, DEC);
    Serial.println(sd.b[d].value, DEC);
  }
}
void sd_init() {
  if (NCD) {
    pinMode(sd.out_pins[0], OUTPUT);
    pinMode(sd.out_pins[1], OUTPUT);
    pinMode(sd.out_pins[2], OUTPUT);
    for (byte k = 0; k < NCD; k++) {
      pinMode (sd.inhib[k], OUTPUT);
    }
  }
}

void sd_write_all_inh_at_state(byte b) {
  for (byte k = 0; k < NCD; k++) {
    digitalWrite (sd.inhib[k], b);
  }
}

void sd_print_state() {
  B *x;
  const byte p = 8 * NCD - 1;
  byte i;
  Serial.print(" [ ");
  for (i = 0; i < 8 * NCD; i++) {
    x  = sd.b + i;
    switch (x->ct) {
      case OUT_OF_ORDER:
        Serial.print("");
        break;
      default:
        Serial.print(x->value, DEC);
        break;
    }
    if (i != p) {
      Serial.print(", ");
    }
  }
  Serial.print(" ] ");
  Serial.println();
}

void sd_update_state() {
  byte a, b, c, n, i;
  int tmp;
  B *x;
  for (n = 0; n < NCD; n++) {
    sd_write_all_inh_at_state(DISABLE);
    digitalWrite (sd.inhib[n], ENABLE);
    for (i = 0; i < 8; i++) {
      x = & sd.b[i + n * 8];
      if (x->ct != OUT_OF_ORDER) {
        a = (1 && (i & B00000001));
        b = (1 && (i & B00000010));
        c = (1 && (i & B00000100));
        digitalWrite(sd.out_pins[0], a);
        digitalWrite(sd.out_pins[1], b);
        digitalWrite(sd.out_pins[2], c);

        b_manage(x, analogRead(sd.read_pin));
      }

//      PLUS LONG s'il existe des OUT_OF_ORDER
      //      a = (1 && (i & B00000001));
      //      b = (1 && (i & B00000010));
      //      c = (1 && (i & B00000100));
      //      digitalWrite(sd.out_pins[0], a);
      //      digitalWrite(sd.out_pins[1], b);
      //      digitalWrite(sd.out_pins[2], c);
      //      if (x->ct != OUT_OF_ORDER) {
      //        b_manage(x, analogRead(sd.read_pin));
      //      }
    }
  }
}
