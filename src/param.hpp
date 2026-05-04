#pragma once
#include <Arduino.h>

// --- PINS ---
#define TEAM_A_PIN      13
#define TEAM_B_PIN      12
#define STARTER_PIN     11   
#define RGB_LEDS_PIN    45

// --- LEDS CONFIG ---
#define NUM_LEDS        3
#define LED_BRIGHTNESS  50   

// --- COULEURS ---
#define COLOR_BLUE      0x005B8C  
#define COLOR_YELLOW    0xF7B500  
#define COLOR_WHITE     0xFFFFFF
#define COLOR_RED       0xFF0000
#define COLOR_GREEN     0x00FF00
#define COLOR_OFF       0x000000

// --- TIMING ---
#define MATCH_DURATION_MS  100000 

// --- COMMUNICATION (AJOUTER CECI) ---
#define MAX_CMD_LEN     32
