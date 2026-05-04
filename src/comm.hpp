#pragma once
#include <Arduino.h>
#include "param.hpp"
#include "match.hpp"

void comm_init();
void comm_update();

void comm_send_starter(bool state);
void comm_send_team(bool isBlue);
void comm_send_reset(); 
