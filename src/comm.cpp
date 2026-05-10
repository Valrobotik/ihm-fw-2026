#include "comm.hpp"
#include "display.hpp"
#include "leds.hpp"
#include "match.hpp"
#include "param.hpp"


// Prototypes
void process_command(String cmd);

void comm_init() {
  Serial.begin(115200);
  // On attend le QXF du maître
}

void comm_update() {
  static String inputString = "";

  while (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == 'F') { // Fin de commande
      process_command(inputString);
      inputString = "";
    } else if (inChar != '\n' && inChar != '\r') {
      inputString += inChar;
    }
  }
}

void process_command(String cmd) {
  if (cmd.length() < 2)
    return;

  String cmd_type = cmd.substring(0, 2);

  // --- RECEPTION (ZDC -> IHM) ---

  // AU : Arrêt d'Urgence (CRITIQUE)
  if (cmd_type == "AU") {
    matchState = EMERGENCY_STOP;
    leds_set_color(COLOR_RED);
    Serial.println("ALERTE: AU RECU DE ZDC !");

    // Force l'affichage immédiat pour être sûr
    display_update_match(0, globalScore, isTeamBlue, matchState);
  }
}
