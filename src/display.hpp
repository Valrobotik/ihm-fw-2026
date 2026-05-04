#pragma once
#include <Arduino.h>
#include "match.hpp" // Pour avoir MatchState

void display_init();
void display_show_logo();
// Nouvelle signature : on passe l'état du match
void display_update_match(int timeRemaining, int score, bool isTeamBlue, MatchState state);
