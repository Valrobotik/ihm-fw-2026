#include "comm.hpp"
#include "display.hpp"
#include "leds.hpp"
#include "match.hpp"
#include "param.hpp"
#include "ros.h"

#include <Arduino.h>
#include <Button2.h>

Button2 btn_a;
Button2 btn_b;
Button2 btn_c;

void BtnTask(void *pvParams) {
  while (1) {
    btn_a.loop();
    btn_b.loop();
    btn_c.loop();
    delay(5);
  }
}

void setup() {
  comm_init();
  init_ros();
  btn_a.begin(BTN_A_PIN, INPUT_PULLUP, false);
  btn_a.setDebounceTime(0);
  btn_a.setClickHandler(handle_a_click);
  btn_a.setDoubleClickHandler(handle_a_double_click);
  btn_a.setTripleClickHandler(handle_a_triple_click);

  btn_b.begin(BTN_B_PIN, INPUT_PULLUP, false);
  btn_b.setClickHandler(handle_b_click);
  btn_b.setDoubleClickHandler(handle_b_double_click);
  btn_b.setDebounceTime(0);

  btn_c.begin(BTN_C_PIN, INPUT_PULLUP, false);
  btn_c.setDebounceTime(0);
  btn_c.setClickHandler(handle_c_click);
  btn_c.setDoubleClickHandler(handle_c_double_click);
  btn_c.setTripleClickHandler(handle_c_triple_click);
  btn_c.setLongClickDetectedHandler(handle_c_long_click);

  xTaskCreatePinnedToCore(BtnTask, "BtnTask", 4096, NULL, 2, NULL, 1);

  leds_init();
  display_init();
  delay(2000);
  match_init();
}

void loop() {
  comm_update();
  match_update();

  // Mise à jour écran (optionnel, peut être déplacé dans display.cpp)
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > 500) {
    lastDisplayUpdate = millis();

    int tempsRestant = 100;
    if (matchState == RUNNING) {
      tempsRestant = 100 - ((millis() - matchStartTime) / 1000);
    } else if (matchState == FINISHED) {
      tempsRestant = 0;
    }

    display_update_match(tempsRestant, globalScore, isTeamBlue, matchState);
  }

  ros_loop();
  delay(5);
}