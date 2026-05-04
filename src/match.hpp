#pragma once
#include <Arduino.h>

// États du match
enum MatchState {
    UNINITIALIZED, // Sécurité : Bloqué tant que pas de QX
    WAITING,       // Prêt, attend le jumper
    RUNNING,       // Match en cours (100s)
    FINISHED,      // Fin de match
    EMERGENCY_STOP // Arrêt d'Urgence (bloquant)
};

// Variables globales
extern MatchState matchState;
extern bool isTeamBlue;
extern unsigned long matchStartTime;
extern bool isRemoteStart;
extern int globalScore;

// Fonctions
void match_init();
void match_update();
bool match_check_starter();
void match_remote_start(); 
