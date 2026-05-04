#include "comm.hpp"
#include "param.hpp"
#include "match.hpp"
#include "leds.hpp"
#include "display.hpp"

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
    if (cmd.length() < 2) return;

    String cmd_type = cmd.substring(0, 2); 
    
    // --- RECEPTION (ZDC -> IHM) ---
    
    // QX : Init / Reset Global
    if (cmd_type == "QX") {
        Serial.println("IHM"); 
        leds_set_color(COLOR_WHITE);
        display_init(); 
        matchState = WAITING; // Débloque le robot
    }
    
    // T0 : Force Equipe BLEUE
    else if (cmd_type == "T0") { 
        isTeamBlue = true;
        leds_set_color(COLOR_BLUE);
        display_update_match(100, globalScore, isTeamBlue, matchState);
    }
    // T1 : Force Equipe JAUNE
    else if (cmd_type == "T1") { 
        isTeamBlue = false;
        leds_set_color(COLOR_YELLOW);
        display_update_match(100, globalScore, isTeamBlue, matchState);
    }
    
    // MS : Start Match (Remote)
    else if (cmd_type == "MS") {
        match_remote_start(); 
        Serial.println("ACK: Match Started by ZDC");
    }
    
    // EM : End Match (Fin forcée)
    else if (cmd_type == "EM") {
        matchState = FINISHED;
        leds_set_color(COLOR_RED);
        Serial.println("ACK: Match Ended by ZDC");
    }
    
    // AU : Arrêt d'Urgence (CRITIQUE)
    else if (cmd_type == "AU") {
        matchState = EMERGENCY_STOP;
        leds_set_color(COLOR_RED);
        Serial.println("ALERTE: AU RECU DE ZDC !");
        
        // Force l'affichage immédiat pour être sûr
        display_update_match(0, globalScore, isTeamBlue, matchState);
    }
    
    // SC : Score Update
    else if (cmd_type == "SC") {
        String valStr = cmd.substring(2);
        globalScore = valStr.toInt(); 
        
        Serial.printf("Score Update: %d\n", globalScore);
        
        int tempsRestant = 100;
        if (matchState == RUNNING) {
             tempsRestant = 100 - ((millis() - matchStartTime) / 1000);
        } else if (matchState == FINISHED) {
             tempsRestant = 0;
        }
        display_update_match(tempsRestant, globalScore, isTeamBlue, matchState);
    }
}

// --- ENVOI (IHM -> ZDC) ---

void comm_send_team(bool isBlue) {
    if (isBlue) Serial.println("TBF"); 
    else Serial.println("TYF");        
}

void comm_send_starter(bool state) {
    if (!state) Serial.println("STF");
}

void comm_send_reset() {
    Serial.println("RSF");
}
