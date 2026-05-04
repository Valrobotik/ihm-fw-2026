#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.hpp"
#include "logo.hpp"
#include "param.hpp"

// Config Ecran
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
#define SCREEN_ADDRESS 0x3C 
#define PIN_SDA 9
#define PIN_SCL 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void display_init() {
    Wire.begin(PIN_SDA, PIN_SCL);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("Erreur SSD1306"));
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display_show_logo();
}

void display_show_logo() {
    display.clearDisplay();
    display.drawBitmap(0, 0, logo_valrobotik, 128, 64, SSD1306_WHITE);
    display.display();
}

void display_update_match(int timeRemaining, int score, bool isTeamBlue, MatchState state) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // CAS 0 : UNINITIALIZED
    if (state == UNINITIALIZED) {
        display.setTextSize(1);
        display.setCursor(16, 5); 
        display.print("SYSTEME NON PRET");
        
        display.setTextSize(2); 
        display.setCursor(22, 25);
        display.print("ATTENTE");
        display.setCursor(30, 45); 
        display.print("ZDC...");
    }

    // CAS 1 : WAITING
    else if (state == WAITING) {
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print(isTeamBlue ? "TEAM: BLEU" : "TEAM: JAUNE");
        
        display.setTextSize(3);
        display.setCursor(28, 20); 
        display.print("PRET");
        
        display.setTextSize(1);
        display.setCursor(20, 52);
        display.print("TIREZ LE JUMPER");
    }

    // CAS 2 : RUNNING
    else if (state == RUNNING) {
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print(isTeamBlue ? "BLEU" : "JAUNE");
        
        int barWidth = map(timeRemaining, 0, 100, 0, 128);
        display.fillRect(0, 10, barWidth, 3, SSD1306_WHITE); 

        display.setTextSize(4);
        int x_pos = (score < 10) ? 52 : (score < 100) ? 40 : 28;
        display.setCursor(x_pos, 18);
        display.print(score);
        
        display.setTextSize(1);
        display.setCursor(105, 42); 
        display.print("pts");
        
        display.setTextSize(1);
        display.setCursor(45, 54);
        display.print("T-");
        display.print(timeRemaining);
        display.print("s");
    }

    // CAS 3 : FINISHED
    else if (state == FINISHED) {
        display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(28, 5); 
        display.print("FIN DU MATCH");
        
        display.setTextSize(3);
        int x_pos = (score < 10) ? 55 : (score < 100) ? 46 : 37;
        display.setCursor(x_pos, 25);
        display.print(score);
        
        display.setTextSize(1);
        display.setCursor(46, 52);
        display.print("POINTS");
    }

    // CAS 4 : EMERGENCY STOP (CRITIQUE)
    else if (state == EMERGENCY_STOP) {
        display.setTextSize(2);
        display.setCursor(35, 10);
        display.print("ARRET");
        
        display.setCursor(20, 35);
        display.print("URGENCE");
        
        display.setTextSize(1);
        display.setCursor(25, 55);
        display.print("STOPPED BY ZDC");
    }

    display.display();
}
