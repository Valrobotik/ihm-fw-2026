#pragma once
#include "match.hpp" // Pour avoir MatchState
#include <Arduino.h>


void display_init();
void display_show_logo();
// Nouvelle signature : on passe l'état du match
void display_update_match(int timeRemaining, int score, bool isTeamBlue,
                          MatchState state);
