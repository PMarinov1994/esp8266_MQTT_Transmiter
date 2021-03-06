#include "ConfigurationManager.h"
#include <LITTLEFS.h>

/*
*	\brief Initialize the component
*
*/
bool CConfigurationManager::Init()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WiFiAPPID, WiFiAPPSK);

    server.begin();
    return true;
}



/*
*	\brief This will loop in main
*
*/
bool CConfigurationManager::HandleConnection()
{
    FlashLed(100);

    WiFiClient client = server.available();
    if (!client)
        return true;

    String req = client.readStringUntil('\r');
    DEBUG_PRINT("Request:");
    DEBUG_PRINT_LN(req);

    if (req.indexOf("/confirm.html") > 0)
        HandleConfiguration(client);
    else
        SendConfigurationContent(client);

    client.flush();
    DEBUG_PRINT_LN("Disconnecting client");

    return true;
}



/*
*	\brief Handle on /confirm.html request
*
*/
void CConfigurationManager::HandleConfiguration(WiFiClient client)
{
    String req = client.readString();

    DEBUG_PRINT_LN(req);

    int postIndex = req.lastIndexOf("\r\n\r\n");
    postIndex += 4;

    DEBUG_PRINT("PostIndex: ");
    DEBUG_PRINT_LN(postIndex);

    DEBUG_PRINT("Request len: ");
    DEBUG_PRINT_LN(req.length());

    if (postIndex >= (int)req.length())
        return;

    String temp = req.substring(postIndex);

    DEBUG_PRINT("just post string: ");
    DEBUG_PRINT_LN(temp);

    CConfigurationFile configFile;
    if (configFile.SetConfiguration(temp))
    {
        String resource = ReadContent(String("/confirm.html"));
        client.print(resource);
    }
    else
        SendConfigurationContent(client);
}



/*
*	\brief Sends a file as responce to client
*
*/
String CConfigurationManager::ReadContent(String resource)
{
    File f = LittleFS.open(resource, "r");
    if (!f)
        return String();

    String s = f.readString();
    f.close();
    
    return s;
}

void CConfigurationManager::SendConfigurationContent(WiFiClient client)
{
    String resource = ReadContent(String("/config.html"));
    
    DEBUG_PRINT_LN("Parsing configuration...");
    
    CConfigurationFile config;
    config.ParseConfiguration();

    DEBUG_PRINT_LN("Setting configuration in responce...");

    int size = resource.length() + 100;
    char* responce = new char[size];
    memset(responce, 0, size);

    DEBUG_PRINT_LN("Calling sprint...");

    sprintf(responce, resource.c_str(),
        config.m_ssid,
        config.m_psk,
        config.m_mqttServerIP,
        config.m_mqttServerPort);

    DEBUG_PRINT_LN(responce);
    client.print(responce);
    delete[] responce;
}



/*
*	\brief Blink onboard LED
*
*/
void CConfigurationManager::FlashLed(int msDelay)
{
    digitalWrite(ESP8266_LED, HIGH);
    delay(msDelay);
    digitalWrite(ESP8266_LED, LOW);
    delay(msDelay);
}
