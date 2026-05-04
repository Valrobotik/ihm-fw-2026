#pragma once

#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/bool.h>
#include <std_msgs/msg/empty.h>
#include <std_msgs/msg/string.h>
#include <std_msgs/msg/int8.h>
#include <rosidl_runtime_c/string_functions.h>

#include "match.hpp"
#include "param.hpp"
#include "leds.hpp"
#include "display.hpp"

// #include "pin_definitions.h"

// ROS
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){Serial.println("Erreur ROS"); return false;}}
#define EXECUTE_EVERY_N_MS(MS, X)  do { \
  static volatile int64_t init = -1; \
  if (init == -1) { init = uxr_millis();} \
  if (uxr_millis() - init > MS) { X; init = uxr_millis();} \
} while (0) \

enum class states {
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
};


void init_ros();
void ros_loop();

bool comm_send_team(bool isBlue);
bool comm_send_starter(bool state);