#include "ConfigurationFile.h"
#include <LITTLEFS.h>

CConfigurationFile::~CConfigurationFile()
{
    if (m_ssid)
        delete[] m_ssid;

    if (m_psk)
        delete[] m_psk;

    if (m_mqttServerIP)
        delete[] m_mqttServerIP;

    if (m_map)
        delete[] m_map;
}

/*
*
*
*/
bool CConfigurationFile::SetConfiguration(String config)
{
    File confFile = LittleFS.open(CONFIG_FILE_NAME, "r");
    if (confFile)
    {
        String config = confFile.readString();
        confFile.close();

        ExtractConfigFileData(config);
    }

    ExtractConfigFileData(config);

    if (!(m_ssid && strlen(m_ssid) > 0) ||
        !(m_psk && strlen(m_psk) > 0) ||
        !(m_mqttServerIP && strlen(m_mqttServerIP) > 0) ||
        !(m_mqttServerPort > 0))
    {
        return false;
    }

    confFile = LittleFS.open(CONFIG_FILE_NAME, "w+");
    if (!confFile)
    {
        DEBUG_PRINT_LN("Fail to create or open file");
        return false;
    }

    String configStr = GetConfigFileData();

    confFile.print(configStr);
    confFile.close();

    return true;
}



/*
*
*
*/
bool CConfigurationFile::ParseConfiguration()
{
    File f = LittleFS.open(CONFIG_FILE_NAME, "r");
    if (!f)
    {
        DEBUG_PRINT_LN("Failed to open file /config.txt");
        return false;
    }

    String config = f.readString();
    f.close();

    ExtractConfigFileData(config);

    DEBUG_PRINT("SSID: ");
    DEBUG_PRINT_LN(m_ssid);

    DEBUG_PRINT("PSK: ");
    DEBUG_PRINT_LN(m_psk);

    DEBUG_PRINT("MQTT: ");
    DEBUG_PRINT_LN(m_mqttServerIP);

    DEBUG_PRINT("MQTT Port: ");
    DEBUG_PRINT_LN(m_mqttServerPort);

    return true;
}


// [orig buffer] <- Pointer1 <- Pointer2
void CConfigurationFile::StringToCharPtr(String str, char ** ch)
{
    if (*ch)
        delete[] * ch;

    unsigned int chLen = str.length() + 1;
    *ch = new char[chLen];
    memset(*ch, 0, chLen);
    str.toCharArray(*ch, chLen);
}



bool CConfigurationFile::ExtractConfigFileData(String str)
{
    unsigned int currPoss = 0;
    unsigned int configLen = str.length();

    String tmpParam;
    String tmpKey;

    if (currPoss == configLen)
        return false;

    StateParser currState = StateParser::WAIT_EQUALS;
    while (true)
    {
        if (currPoss >= configLen)
            break;

        char ch = str.charAt(currPoss++);

        if (CONFIG_FILE_EQUALS == ch)
            currState = StateParser::GOT_EQUALS;

        if (CONFIG_FILE_SEPAREATOR == ch)
            currState = StateParser::GOT_SEPARATOR;

        if (currPoss >= configLen)
            currState = StateParser::END_OF_FILE;

        switch (currState)
        {
        case StateParser::WAIT_EQUALS:

            tmpKey += ch;
            break;

        case StateParser::GOT_EQUALS:

            currState = StateParser::WAIT_SEPARATOR;
            break;

        case StateParser::WAIT_SEPARATOR:

            tmpParam += ch;
            break;

        case StateParser::END_OF_FILE:

            // Fall through to parse the last param.
            // Append char if it is not an EQUAL sign.
            if (CONFIG_FILE_EQUALS != ch)
                tmpParam += ch;

        case StateParser::GOT_SEPARATOR:

            if (tmpParam.length() == 0)
                return false;

            SetConfigParam(tmpParam, tmpKey);
            tmpParam = "";
            tmpKey = "";
            currState = StateParser::WAIT_EQUALS;
            break;
        }
    }

    return true;
}



void CConfigurationFile::SetConfigParam(String configParam, String key)
{
    DEBUG_PRINT("Config param: ");
    DEBUG_PRINT(key);
    DEBUG_PRINT(" Value: ");
    DEBUG_PRINT_LN(configParam);

    for (int i = 0; i < m_mapSize; ++i)
    {
        if (m_map[i].m_key != key)
            continue;

        switch (m_map[i].m_type)
        {
        case ParamType::CHAR_ARRAY:
            StringToCharPtr(configParam, m_map[i].m_charArr);
            DEBUG_PRINT("Char member: ");
            DEBUG_PRINT_LN(*m_map[i].m_charArr);
            break;
        case ParamType::INT:
            *(m_map[i].m_int) = configParam.toInt();
            DEBUG_PRINT("Int member: ");
            DEBUG_PRINT_LN(*m_map[i].m_int);
            break;
        }
    }
}



String CConfigurationFile::GetConfigFileData()
{
    String result;

    for (int i = 0; i < m_mapSize; ++i)
    {
        result += m_map[i].m_key;
        result += CONFIG_FILE_EQUALS;

        switch (m_map[i].m_type)
        {
        case ParamType::CHAR_ARRAY:

            result += *(m_map[i].m_charArr);
            break;
        case ParamType::INT:

            result += *(m_map[i].m_int);
            break;
        }

        if (i < m_mapSize - 1)
            result += CONFIG_FILE_SEPAREATOR;
    }

    return result;
}
