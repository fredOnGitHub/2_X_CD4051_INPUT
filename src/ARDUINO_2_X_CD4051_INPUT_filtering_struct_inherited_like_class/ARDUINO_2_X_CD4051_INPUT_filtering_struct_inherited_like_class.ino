
#include "constants.h"
#include "structs.hpp"

/********************************************************************/

#if not defined (TEST)

ARDUINO_B ARRAY_ARDUINO_B[] = {
  {PIN2, ARDUINO_PIN_INPUT_PULLUP, CHAN8, NOTE_ON, CONTROL1}
};
AB ab{ARRAY_ARDUINO_B, sizeof(ARRAY_ARDUINO_B) / sizeof(ARDUINO_B)};

ARDUINO_P ARRAY_ARDUINO_P[] = {
  {A1, CHAN8, NOTE_ON, CONTROL2},
  {A2, CHAN8, NOTE_ON, CONTROL3}
};
AP ap{ARRAY_ARDUINO_P, sizeof(ARRAY_ARDUINO_P) / sizeof(ARDUINO_P)};


CD4051_B ARRAY_CD4051_B[] = {
  {14, CD4051_PULL_DOWN, CHAN8, NOTE_ON, CONTROL12},
};
CB cb{ARRAY_CD4051_B, sizeof(ARRAY_CD4051_B) / sizeof(CD4051_B)};

CD4051_P ARRAY_CD4051_P[] = {
  {0, CHAN8, NOTE_ON, CONTROL11},
  {15, CHAN8, NOTE_ON, CONTROL13}
};
CP cp{ARRAY_CD4051_P, sizeof(ARRAY_CD4051_P) / sizeof(CD4051_P)};


PILOT_CD4051 pcd{
  A0,
  {8, 9, 10},
  {6, 7},
  &cb,
  &cp
};
void setup() {
  Serial.begin(115200);
  ab.init();
  pcd.init();
}

void loop() {
  ab.action();
  ap.action();
  pcd.action();
}

#else

/********************************************************************/

void for_test() {
  sc("---------------ARDUINO_B-----------------");
  ARDUINO_B ab {PIN2, ARDUINO_PIN_INPUT_PULLUP, CHAN8, NOTE_ON, CONTROL1};
  ab.pr();
  
  sc("---------------ARDUINO_P-----------------");
  ARDUINO_P ap {A2, CHAN8, NOTE_ON, CONTROL3};
  ap.pr();

  sc("---------------CD4051_B-----------------");
  CD4051_B cb {14, CD4051_PULL_DOWN, CHAN8, NOTE_ON, CONTROL12};
  cb.pr();

  sc("---------------CD4051_P-----------------");
  CD4051_P cp {15, CHAN8, NOTE_ON, CONTROL13};
  cp.pr();
}

void setup() {
  Serial.begin(115200);
  for_test();
}

void loop() {}

#endif
