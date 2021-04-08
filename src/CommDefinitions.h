#ifndef COMMDEFINITIONS__H
#define COMMDEFINITIONS__H

/*
* COMMAND REQUEST FORMAT:
* |{request id}:[command id] [arg name] [arg value]|
*
* {} - optional
* [] - mandatory
*
* COMMAND RESULT:
* |[REQUEST ID]:[COMMAND RESULT]|
*
* If the request ID is not given, it will be auto generated
* with the current possition number in the command queue.
*
*/

#define SERIAL_COMMUNICATION_SPEED 115200

#define MESSAGE_START '|'
#define MESSAGE_END '|'

#define COMMAND_ARG_SEPARATOR ' '
#define COMMAND_ID_SEPARATOR ':'

#define RESPONSE_ID_RESULT_SEPARATOR ':'

#define SUCCESS 0

#define CMD_UNKNOWN 0
#define ERROR_UNKNOWN_CMD 0xFF

#define INIT_DONE_ID     0xCD
#define INIT_DONE_RESULT 0xCD

// ============================================================================================================

// COMMAND
#define CMD_LOAD_FILE_CONFIG 1

// ERRORS
#define ERROR_CMD_LOAD_FILE_CONFIG_OPEN_FILE 1

// ============================================================================================================

// COMMAND
#define CMD_WIFI_CONNECT_FROM_FILE 11
#define CMD_WIFI_CONNECT 22
#define CMD_WIFI_CONNECT_SSID_ARG "-ssid"
#define CMD_WIFI_CONNECT_PSK_ARG "-psk"

// ERRORS
// ERRORS are same as wl_status_t in wl_definitions.h
#define ERROR_WIFI_NO_SHIELD        255   // for compatibility with WiFi Shield library
#define ERROR_WIFI_IDLE_STATUS      0
#define ERROR_WIFI_NO_SSID_AVAIL    1
#define ERROR_WIFI_SCAN_COMPLETED   2
#define ERROR_WIFI_CONNECTED        3
#define ERROR_WIFI_CONNECT_FAILED   4
#define ERROR_WIFI_CONNECTION_LOST  5
#define ERROR_WIFI_DISCONNECTED     6

#define ERROR_CMD_WIFI_CONNECT_SSID_ARG_MISSING 8
#define ERROR_CMD_WIFI_CONNECT_PSK_ARG_MISSING  9

#define ERROR_WIFI_INIT_FAILED 10

// ============================================================================================================

// COMMAND
#define CMD_MQTT_INIT_FROM_FILE 111
#define CMD_MQTT_INIT 222
#define CMD_MQTT_INIT_IP_ARG "-ip"
#define CMD_MQTT_INIT_PORT_ARG "-p"

// ERRORS
// ERRORS are same as PubSubClient.h defines.
#ifndef PubSubClient_h
#define MQTT_CONNECTION_TIMEOUT     -4
#define MQTT_CONNECTION_LOST        -3
#define MQTT_CONNECT_FAILED         -2
#define MQTT_DISCONNECTED           -1
#define MQTT_CONNECTED               0
#define MQTT_CONNECT_BAD_PROTOCOL    1
#define MQTT_CONNECT_BAD_CLIENT_ID   2
#define MQTT_CONNECT_UNAVAILABLE     3
#define MQTT_CONNECT_BAD_CREDENTIALS 4
#define MQTT_CONNECT_UNAUTHORIZED    5
#endif

#define ERROR_CMD_MQTT_INIT_IP_ARG_MISSING   1
#define ERROR_CMD_MQTT_INIT_IP_ARG_INVALID   2
#define ERROR_CMD_MQTT_INIT_PORT_ARG_MISSING 3
#define ERROR_CMD_MQTT_INIT_PORT_ARG_INVALID 4

// ============================================================================================================

// COMMAND
#define CMD_MQTT_PUBLISH 2
#define CMD_MQTT_PUBLISH_TOPIC_ARG "-t"
#define CMD_MQTT_PUBLISH_MESSAGE_ARG "-m"

// ERRORS
#define ERROR_CMD_MQTT_PUBLISH_TOPIC_ARG_MISSING 1
#define ERROR_CMD_MQTT_PUBLISH_MESSAGE_ARG_MISSING 2
#define ERROR_CMD_MQTT_PUBLISH_CLIENT_NOT_CONNECTED 3
#define ERROR_CMD_MQTT_PUBLISH_CONN_LOST_OR_MSG_TOO_LARGE 4

// ============================================================================================================

// COMMAND
#define CMD_MQTT_DISCONECT 10

// ERRORS

// ============================================================================================================

// COMMAND
#define CMD_WIFI_DISCONECT 100

// ERRORS
#define ERROR_CMD_WIFI_DISCONECT_DC_FAILED 1
#define ERROR_CMD_WIFI_DISCONECT_POWER_DOWN_WIFI_FAILED 2

// ============================================================================================================

#endif