#include <Arduino.h>
#include "param.hpp"
#include "leds.hpp"
#include "match.hpp"
#include "comm.hpp"
#include "display.hpp"

void setup() {
    comm_init();
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
    
    delay(5);}