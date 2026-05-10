#include "ros.h"

states state;

rcl_publisher_t publisher_team;
rcl_publisher_t publisher_start;
std_msgs__msg__String msg_team;
std_msgs__msg__Int8 msg_start;
std_msgs__msg__String received_msg_team;
std_msgs__msg__Empty received_msg_zdc_handshake;
std_msgs__msg__Int8 received_msg_state;
std_msgs__msg__Int32 received_msg_score;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_subscription_t subscriber_team;
rcl_subscription_t subscriber_zdc_handshake;
rcl_subscription_t subscriber_state;
rcl_subscription_t subscriber_score;
rclc_executor_t executor;

void init_ros() {
  Serial.printf("Connecting to ap: %s\n", ENV_WIFI_SSID);
  IPAddress agent_ip(ENV_AGENT_IP);
  uint16_t agent_port = 8888;
  set_microros_wifi_transports(ENV_WIFI_SSID, ENV_WIFI_PASSWORD, agent_ip, agent_port);
  WiFi.setAutoReconnect(true);
  state = states::WAITING_AGENT;
}

void ZdcHandshakeCallback(const void* msgin) {
  if (matchState = WAITING) return;
    const std_msgs__msg__Empty* msg = (const std_msgs__msg__Empty*)msgin;
    Serial.println("IHM");
    leds_set_color(COLOR_WHITE);
    display_init();
    matchState = WAITING; // Débloque le robot
}

void TeamCallback(const void* msgin) {
  const std_msgs__msg__String* msg = (const std_msgs__msg__String*)msgin;
  if (!strcmp(msg->data.data, "blue")) {
    isTeamBlue = true;
    leds_set_color(COLOR_BLUE);
    display_update_match(100, globalScore, isTeamBlue, matchState);
  } else {
    isTeamBlue = false;
    leds_set_color(COLOR_YELLOW);
    display_update_match(100, globalScore, isTeamBlue, matchState);
  }
}

void StateCallback(const void* msgin) {
  const std_msgs__msg__Int8* msg = (const std_msgs__msg__Int8*)msgin;
  switch (msg->data) {
    case 3:
      matchState = FINISHED;
      leds_set_color(COLOR_RED);
      Serial.println("ACK: Match Ended by ZDC");
      break;
  }
}

void ScoreCallback(const void* msgin) {
  const std_msgs__msg__Int32* msg = (const std_msgs__msg__Int32*)msgin;
  globalScore = msg->data;
  Serial.printf("Score Update: %d\n", globalScore);
  int tempsRestant = 100;
  if (matchState == RUNNING) {
    tempsRestant = 100 - ((millis() - matchStartTime) / 1000);
  } else if (matchState == FINISHED) {
    tempsRestant = 0;
  }
  display_update_match(tempsRestant, globalScore, isTeamBlue, matchState);
}

bool create_entities() {
  allocator = rcl_get_default_allocator();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  RCCHECK(rcl_init_options_init(&init_options, allocator));
  RCCHECK(rcl_init_options_set_domain_id(&init_options, 42));

  RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator));
  RCCHECK(rclc_node_init_default(&node, "actionneurs", "", &support));

  RCCHECK(rclc_publisher_init_default(
    &publisher_team,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "/team"));

  RCCHECK(rclc_publisher_init_default(
    &publisher_start,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8),
    "/state"));

  RCCHECK(rclc_executor_init(&executor, &support.context, 3, &allocator));

  std_msgs__msg__String__init(&received_msg_team);
  received_msg_team.data.data = (char*) malloc(10);
  received_msg_team.data.capacity = 10;
  received_msg_team.data.size = 0;
  RCCHECK(rclc_subscription_init_default(&subscriber_team, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "/team"));
  RCCHECK(rclc_subscription_init_default(&subscriber_zdc_handshake, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Empty),
    "/zdc_vision/handshake"));
  RCCHECK(rclc_subscription_init_default(&subscriber_state, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int8),
    "/state"));
  RCCHECK(rclc_subscription_init_default(&subscriber_score, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
    "/score"));

  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_team, &received_msg_team,
      &TeamCallback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_zdc_handshake, &received_msg_zdc_handshake,
      &ZdcHandshakeCallback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_score, &received_msg_score,
      &ScoreCallback, ON_NEW_DATA));

  return true;
}

void destroy_entities() {
  rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
  (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

  (void) rcl_publisher_fini(&publisher_team, &node);
  (void) rcl_publisher_fini(&publisher_start, &node);
  (void) rclc_executor_fini(&executor);
  (void) rcl_subscription_fini(&subscriber_team, &node);
  (void) rcl_subscription_fini(&subscriber_zdc_handshake, &node);
  (void) rcl_subscription_fini(&subscriber_state, &node);
  (void) rcl_subscription_fini(&subscriber_score, &node);
  (void) rcl_node_fini(&node);
  rclc_support_fini(&support);

  std_msgs__msg__String__fini(&received_msg_team);
}

void ros_loop() {
  switch (state) {
  case states::WAITING_AGENT:
    EXECUTE_EVERY_N_MS(500, state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1))
                                        ? states::AGENT_AVAILABLE
                                        : states::WAITING_AGENT;);
    break;
  case states::AGENT_AVAILABLE:
    state = (true == create_entities()) ? states::AGENT_CONNECTED : states::WAITING_AGENT;
    if (state == states::WAITING_AGENT) {
      destroy_entities();
    };
    break;
  case states::AGENT_CONNECTED:
    EXECUTE_EVERY_N_MS(200, state = (RMW_RET_OK == rmw_uros_ping_agent(100, 1))
                                        ? states::AGENT_CONNECTED
                                        : states::AGENT_DISCONNECTED;);
    if (state == states::AGENT_CONNECTED) {
        // EXECUTE_EVERY_N_MS(100, ros_update_obstacle());
        // EXECUTE_EVERY_N_MS(100, ros_update_odometry());
        // EXECUTE_EVERY_N_MS(1000, ros_update_batt());
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    }
    break;
  case states::AGENT_DISCONNECTED:
    destroy_entities();
    state = states::WAITING_AGENT;
    break;
  default:
    break;
  }
}

bool comm_send_team(bool isBlue) {
  if (isBlue) {
    rosidl_runtime_c__String__assign(&msg_team.data, "blue");
  } else {
    rosidl_runtime_c__String__assign(&msg_team.data, "yellow");
  }
  RCCHECK(rcl_publish(&publisher_team, &msg_team, NULL));
  return true;
}

bool comm_send_starter(bool state) {
  if (state) {
    return true;
  }
  msg_start.data = 2;
  RCCHECK(rcl_publish(&publisher_start, &msg_start, NULL));
  return true;
}

bool comm_send_reset() {
  msg_start.data = 3;
  RCCHECK(rcl_publish(&publisher_start, &msg_start, NULL));
  return true;
}