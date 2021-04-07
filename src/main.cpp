#include <Arduino.h>

// FileSystem headers
#include <FS.h>
#include <LITTLEFS.h>

// Project headers
#include "ConfigurationManager.h"
#include "BootManager.h"
#include "WorkingStation.h"
#include "GlobalDefinitions.h"

// Setup variables
CConfigurationManager configurationManager;
CWorkingStation workStation;

bool isConfigurationMode = false;

void setup()
{
    CBootManager bootManager;
    isConfigurationMode = bootManager.IsConfigBoot();

    delay(20);

    // This depends on the Global #define PRINT_LINES
    // * 0 Serial monitor will be disable.
    //   TX will be set to HIGH and power the I2C devices.
    // * 1 Serial monitor will only transmit (TX is active)
    //   No devices should be connected to I2C power line.
    // ** RX will always be input GPIO for firmware boot mode.
    //    Has a hardware PULL_UP resistor.
    // *** LOW Firmware starts web server for configurations.
    // *** HIGHT (Normal) Firmware will read and post sensor data.
    Serial.begin(115200);
    Serial.println("SETUP");

    bool littleFs = LittleFS.begin();
    DEBUG_PRINT("SPIFFS Status: ");
    DEBUG_PRINT_LN(littleFs);

    DEBUG_PRINT("Booting mode: ");
    DEBUG_PRINT_LN(isConfigurationMode == true ? "configuration" : "operation");

    if (isConfigurationMode)
        configurationManager.Init();

    DEBUG_PRINT_LN("========================================================");
    DEBUG_PRINT_LN("========================================================");
    DEBUG_PRINT_LN("========================================================");
}

void loop()
{
    if (isConfigurationMode)
        configurationManager.HandleConnection();
    else
        workStation.HandleCommands();

    delay(20);
}