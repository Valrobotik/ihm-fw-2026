#pragma once
#include "match.hpp"
#include "param.hpp"
#include <Arduino.h>


void comm_init();
void comm_update();

void comm_send_starter(bool state);
void comm_send_team(bool isBlue);
void comm_send_reset();
