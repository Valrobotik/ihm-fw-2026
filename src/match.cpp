#include "match.hpp"
#include "comm.hpp"
#include "leds.hpp"
#include "param.hpp"
#include "ros.h"


// Variables globales
MatchState matchState = UNINITIALIZED; // Bloqué par défaut
bool isTeamBlue = true;
unsigned long matchStartTime = 0;
bool isRemoteStart = false;
int globalScore = 0;

static int lastTeamA = -1;
static unsigned long lastSwitchTime = 0;

void match_init() {
  pinMode(TEAM_A_PIN, INPUT_PULLUP);
  pinMode(TEAM_B_PIN, INPUT_PULLUP);
  pinMode(STARTER_PIN, INPUT_PULLUP);

  // Check Jumper au boot
  if (digitalRead(STARTER_PIN) == LOW) {
    leds_set_color(COLOR_RED);
    Serial.println("ALERTE: Jumper absent au démarrage !");
    while (digitalRead(STARTER_PIN) == LOW) {
      delay(100);
    }
    Serial.println("Jumper remis. Prêt.");
  }

  // Init état switch
  if (digitalRead(TEAM_A_PIN) == LOW)
    isTeamBlue = true;
  else if (digitalRead(TEAM_B_PIN) == LOW)
    isTeamBlue = false;
  lastTeamA = digitalRead(TEAM_A_PIN);
}

void match_read_team() {
  int currentStateA = digitalRead(TEAM_A_PIN);

  if (lastTeamA == -1) {
    lastTeamA = currentStateA;
    return;
  }

  if (currentStateA != lastTeamA) {
    if (millis() - lastSwitchTime < 200)
      return;
    lastSwitchTime = millis();

    isTeamBlue = !isTeamBlue;

    Serial.print("Switch -> ");
    Serial.println(isTeamBlue ? "BLEU" : "JAUNE");

    comm_send_team(isTeamBlue);
    lastTeamA = currentStateA;
  }
}

bool match_check_starter() { return (digitalRead(STARTER_PIN) == HIGH); }

void match_remote_start() {
  if (matchState == WAITING || matchState == FINISHED) {
    matchState = RUNNING;
    matchStartTime = millis();
    isRemoteStart = true;
    leds_set_color(COLOR_WHITE);
    Serial.println("GO ! Remote Start (ZDC)");
  }
}

void match_update() {
  bool jumperPresent = match_check_starter();

  // 1. GESTION REMOTE FLAG
  if (isRemoteStart && !jumperPresent) {
    isRemoteStart = false;
    Serial.println("INFO: Jumper retiré, sécurité réarmée.");
  }

  // 2. RESET PRIORITAIRE
  // IMPORTANT : On NE sort PAS de EMERGENCY_STOP avec le jumper ! (Seul QX le
  // peut)
  if (matchState != UNINITIALIZED && matchState != EMERGENCY_STOP &&
      matchState != WAITING && jumperPresent && !isRemoteStart) {

    matchState = WAITING;
    isRemoteStart = false;

    leds_set_color(COLOR_GREEN);
    Serial.println("RESET: Jumper remis. Arrêt d'urgence.");

    comm_send_reset();
    delay(500);
    return;
  }

  switch (matchState) {
  case UNINITIALIZED:
    match_read_team();
    // Blink Rouge lent
    static unsigned long lastBlinkUni;
    static bool blinkUni;
    if (millis() - lastBlinkUni > 1000) {
      lastBlinkUni = millis();
      blinkUni = !blinkUni;
      leds_set_color(blinkUni ? COLOR_RED : COLOR_OFF);
    }
    break;

  case WAITING:
    match_read_team();
    leds_set_color(isTeamBlue ? COLOR_BLUE : COLOR_YELLOW);

    if (!jumperPresent) {
      delay(50);
      if (!match_check_starter()) {
        matchState = RUNNING;
        matchStartTime = millis();
        isRemoteStart = false;

        Serial.println("GO ! Start Manuel");
        leds_set_color(COLOR_WHITE);
        comm_send_starter(false);
        delay(300);
      }
    }
    break;

  case RUNNING:
    if (millis() - matchStartTime >= MATCH_DURATION_MS) {
      matchState = FINISHED;
      Serial.println("FIN DU MATCH !");
    } else {
      leds_set_color(isTeamBlue ? COLOR_BLUE : COLOR_YELLOW);

      static unsigned long lastLog = 0;
      if (millis() - lastLog > 1000) {
        lastLog = millis();
        Serial.printf("Temps: %d s\n",
                      (MATCH_DURATION_MS - (millis() - matchStartTime)) / 1000);
      }
    }
    break;

  case FINISHED:
    static bool blink = false;
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 500) {
      lastBlink = millis();
      blink = !blink;
      leds_set_color(blink ? COLOR_RED : COLOR_OFF);
    }
    break;

  case EMERGENCY_STOP:
    // BLOCAGE TOTAL
    // Strobe rouge rapide
    static bool blinkAU = false;
    static unsigned long lastBlinkAU = 0;
    if (millis() - lastBlinkAU > 100) { // 10Hz
      lastBlinkAU = millis();
      blinkAU = !blinkAU;
      leds_set_color(blinkAU ? COLOR_RED : COLOR_OFF);
    }
    // On ne fait RIEN d'autre ici, le robot est mort tant qu'il n'est pas reset
    // (QX)
    break;
  }
}
