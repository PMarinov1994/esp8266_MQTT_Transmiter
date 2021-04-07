#ifndef __GlobalDefinitions__H
#define __GlobalDefinitions__H

#pragma once

// CConfigurationManager Definitions
#define ESP8266_LED 2
#define BOOT_PIN 2

const char stationID[] = "D0DEA2CF-7AFA-42FF-B887-9DE055ECA22C";

const char WiFiAPPID[] = "ESP8266 ConfigWifi";
const char WiFiAPPSK[] = "123456789";

// CConfigurationFile Definitions
#define CONFIG_FILE_NAME "/config.txt"
#define CONFIG_FILE_SEPAREATOR '&'
#define CONFIG_FILE_EQUALS '='
#define CONFIG_FILE_END '#'

// Real Global Definitions
#define PRINT_LINES 0

#if (PRINT_LINES == 1)
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINT_LN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x) ((void)0)
#define DEBUG_PRINT_LN(x) ((void)0)
#endif // DEBUG

#define ESP8266_BATTERY_LOW_ERROR 2

#define ESP_MIN_OPERATE_VOLTAGE 2.8

#define SEALEVELPRESSURE_HPA (1013.25)

#endif // !__GlobalDefinitions__H
