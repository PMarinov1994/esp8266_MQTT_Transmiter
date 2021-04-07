#include "WorkingStation.h"
#include "ConfigurationFile.h"
#include "CommDefinitions.h"

CWorkingStation::CWorkingStation()
    : m_client(m_espClient), m_currState(MESSAGE_PARSER::WAIT_START){};

void CWorkingStation::HandleCommands()
{
    this->FetchCommands();
    if (this->HasCommands())
    {
        int id;
        int result = this->ExecuteCommand(&id);

        String resultContent;
        resultContent += MESSAGE_START;
        resultContent += String(id);
        resultContent += RESPONSE_ID_RESULT_SEPARATOR;
        resultContent += String(result);
        resultContent += MESSAGE_END;

        Serial.println(resultContent);
    }
}

/*
*	\brief Initialize the component
*
*/
bool CWorkingStation::InitFromFS()
{
    CConfigurationFile configFile;
    if (!configFile.ParseConfiguration())
        return false;

    int ssidLen = strlen(configFile.m_ssid) + 1;
    int pskLen = strlen(configFile.m_psk) + 1;

    if (m_ssid)
        delete[] m_ssid;

    m_ssid = new char[ssidLen];

    if (m_psk)
        delete[] m_psk;

    m_psk = new char[pskLen];

    memcpy(m_ssid, configFile.m_ssid, ssidLen);
    memcpy(m_psk, configFile.m_psk, pskLen);

    m_mqttServerIPAddr.fromString(configFile.m_mqttServerIP);
    m_port = configFile.m_mqttServerPort;

    return true;
}

bool CWorkingStation::HasCommands()
{
    return !m_commandQueue.empty();
}

void CWorkingStation::FetchCommands()
{
    switch (m_currState)
    {
    case MESSAGE_PARSER::WAIT_START:

        if (Serial.available() > 0)
        {
            char ch;
            Serial.read(&ch, 1);

            if (MESSAGE_START == ch)
            {
                m_currState = MESSAGE_PARSER::WAIT_END;
                break;
            }
        }

        break;

    case MESSAGE_PARSER::WAIT_END:

        if (Serial.available() > 0)
        {
            char ch;
            Serial.read(&ch, 1);

            if (MESSAGE_END == ch)
            {
                m_currState = MESSAGE_PARSER::PARSE_MESSAGE;
                break;
            }

            m_messageBuffer += ch;
        }

        break;

    case MESSAGE_PARSER::PARSE_MESSAGE:
        int buffLen = m_messageBuffer.length();

        int commandEnd = m_messageBuffer.indexOf(COMMAND_ARG_SEPARATOR);
        if (commandEnd == -1)
            commandEnd = buffLen;

        String tmpFullCommand = m_messageBuffer.substring(0, commandEnd);
        int idPoss = tmpFullCommand.indexOf(COMMAND_ID_SEPARATOR);

        int id = -1;
        String tmpCommand;
        if (idPoss != -1)
        {
            char *pEnd;

            String idStr = tmpFullCommand.substring(0, idPoss);
            id = strtol(idStr.c_str(), &pEnd, 10);

            tmpCommand = tmpFullCommand.substring(idPoss + 1);
        }
        else
        {
            tmpCommand = tmpFullCommand;
        }

        int currPoss = commandEnd + 1;

        std::vector<String> params;
        while (currPoss < buffLen)
        {
            int lastPoss = m_messageBuffer.indexOf(COMMAND_ARG_SEPARATOR, currPoss);

            // For the last parameter, the index will be -1. We will get the
            // rest of the message in one go.
            if (lastPoss == -1)
                lastPoss = buffLen;

            String param = m_messageBuffer.substring(currPoss, lastPoss);
            params.push_back(param);

            currPoss = lastPoss + 1;
        }

        tzCOMMAND *cmd = new tzCOMMAND();
        std::map<String, String> tmpArgs;

        size_t paramsSize = params.size();
        if (paramsSize > 0 && paramsSize % 2 != 0)
        {
            // TODO: Handle parameters error.
            goto CLEAN_UP;
        }
        else if (paramsSize > 0)
        {
            std::vector<String>::iterator it = params.begin();
            while (it != params.end())
            {
                String name = *it;
                ++it;

                String value = *it;
                ++it;

                std::pair<String, String> entry(name, value);
                tmpArgs.insert(entry);
            }
        }

        cmd->m_id = id != -1 ? id : (int)m_commandQueue.size();
        cmd->m_command = tmpCommand;
        cmd->m_args = tmpArgs;

        m_commandQueue.push(cmd);

    RESET_MSG_PARSER:
        DEBUG_PRINT_LN("RESET_MSG_PARSER.");
        m_currState = MESSAGE_PARSER::WAIT_START;
        m_messageBuffer.clear();
        break;

    CLEAN_UP:
        DEBUG_PRINT_LN("CLEAN_UP.");
        delete cmd;
        goto RESET_MSG_PARSER;
    }
}

int CWorkingStation::ExecuteCommand(int *outCmdId)
{
    int result = 0;

    tzCOMMAND *cmd = m_commandQueue.front();
    m_commandQueue.pop();

    *outCmdId = cmd->m_id;

    char *pEnd;
    int cmdId = strtol(cmd->m_command.c_str(), &pEnd, 10);

    DEBUG_PRINT("cmdId: ");
    DEBUG_PRINT(cmd->m_id);
    DEBUG_PRINT(" Exec cmd: ");
    DEBUG_PRINT(cmdId);
#if (PRINT_LINES == 1)
    std::map<String, String>::iterator it = cmd->m_args.begin();
    while (it != cmd->m_args.end())
    {
        DEBUG_PRINT(" argName: ");
        DEBUG_PRINT(it->first);
        DEBUG_PRINT(" argParam: ");
        DEBUG_PRINT(it->second);

        it++;
    }

    DEBUG_PRINT_LN("");

#endif

    switch (cmdId)
    {

    case CMD_MQTT_PUBLISH:
    {
        std::map<String, String>::iterator it = cmd->m_args.find(String(CMD_MQTT_PUBLISH_TOPIC_ARG));
        if (it == cmd->m_args.end())
        {
            result = ERROR_CMD_MQTT_PUBLISH_TOPIC_ARG_MISSING;
            break;
        }

        String topic = it->second;

        it = cmd->m_args.find(String(CMD_MQTT_PUBLISH_MESSAGE_ARG));
        if (it == cmd->m_args.end())
        {
            result = ERROR_CMD_MQTT_PUBLISH_MESSAGE_ARG_MISSING;
            break;
        }

        String message = it->second;

        result = this->CmdMqttPublish(topic.c_str(), message.c_str());
        break;
    }

    case CMD_WIFI_CONNECT_FROM_FILE:
        result = this->ConnectToWifi();
        break;

    case CMD_WIFI_CONNECT:
    {
        std::map<String, String>::iterator it = cmd->m_args.find(CMD_WIFI_CONNECT_SSID_ARG);
        if (it == cmd->m_args.end())
        {
            result = ERROR_CMD_WIFI_CONNECT_SSID_ARG_MISSING;
            break;
        }

        String ssid = it->second;

        it = cmd->m_args.find(CMD_WIFI_CONNECT_PSK_ARG);
        if (it == cmd->m_args.end())
        {
            result = ERROR_CMD_WIFI_CONNECT_PSK_ARG_MISSING;
            break;
        }

        String psk = it->second;

        int ssidLen = ssid.length() + 1;
        int pskLen = psk.length() + 1;

        if (m_ssid)
            delete[] m_ssid;

        m_ssid = new char[ssidLen];

        if (m_psk)
            delete[] m_psk;

        m_psk = new char[pskLen];

        memcpy(m_ssid, ssid.c_str(), ssidLen);
        memcpy(m_psk, psk.c_str(), pskLen);

        result = this->ConnectToWifi();
        break;
    }

    case CMD_MQTT_INIT_FROM_FILE:
        result = this->ConnectToMQTT();
        break;

    case CMD_MQTT_INIT:
    {
        std::map<String, String>::iterator it = cmd->m_args.find(CMD_MQTT_INIT_IP_ARG);
        if (it == cmd->m_args.end())
        {
            result = ERROR_CMD_MQTT_INIT_IP_ARG_MISSING;
            break;
        }

        String ip = it->second;

        it = cmd->m_args.find(CMD_MQTT_INIT_PORT_ARG);
        if (it == cmd->m_args.end())
        {
            result = ERROR_CMD_MQTT_INIT_PORT_ARG_MISSING;
            break;
        }

        String port = it->second;

        if (!m_mqttServerIPAddr.fromString(ip))
        {
            result = ERROR_CMD_MQTT_INIT_IP_ARG_INVALID;
            break;
        }

        m_port = strtol(port.c_str(), &pEnd, 10);
        if (m_port == 0)
        {
            result = ERROR_CMD_MQTT_INIT_PORT_ARG_INVALID;
            break;
        }

        result = this->ConnectToMQTT();
        break;
    }

    case CMD_LOAD_FILE_CONFIG:
        if (this->InitFromFS())
            result = SUCCESS;
        else
            result = ERROR_CMD_LOAD_FILE_CONFIG_OPEN_FILE;

        break;

    case CMD_MQTT_DISCONECT:
        if (m_client.connected())
            m_client.disconnect();

        result = SUCCESS;
        break;

    case CMD_WIFI_DISCONECT:
        result = this->DisconnectFromWifi();
        break;

    case CMD_UNKNOWN:
    default:
        result = ERROR_UNKNOWN_CMD;
        break;
    }

    delete cmd;
    return result;
}

/*
*	\brief This will loop in main
*/
int CWorkingStation::ConnectToMQTT()
{
    int tryCounter = 0;
    m_client.setServer(m_mqttServerIPAddr, m_port);

    while (!m_client.connected())
    {
        if (++tryCounter > 4)
            break;

        if (m_client.connect(stationID))
        {
            DEBUG_PRINT_LN("Client connected!");
            return SUCCESS;
        }
        else
        {
            DEBUG_PRINT("failed, rc=");
            DEBUG_PRINT(m_client.state());
            DEBUG_PRINT_LN(" try again in 2 seconds");

            // Wait 2 seconds before retrying
            delay(2000);
        }
    }

    return m_client.state();
}

int CWorkingStation::ConnectToWifi()
{
    if (!WiFi.mode(WIFI_STA))
        return ERROR_WIFI_INIT_FAILED;

    delay(20);

    if (WL_CONNECT_FAILED == WiFi.begin(m_ssid, m_psk))
        return WL_CONNECT_FAILED;

    int tries = 0;
    DEBUG_PRINT("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        DEBUG_PRINT(".");

        if (tries++ > 10)
        {
            DEBUG_PRINT_LN("Failed to connect! Enter deep sleep mode.");
            return WiFi.status();
        }
    }

    DEBUG_PRINT_LN("");
    DEBUG_PRINT("Connected, IP address: ");
    DEBUG_PRINT_LN(WiFi.localIP());

    return SUCCESS;
}

int CWorkingStation::DisconnectFromWifi()
{
    if (m_client.connected())
        m_client.disconnect();

    if (!WiFi.disconnect())
        return ERROR_CMD_WIFI_DISCONECT_DC_FAILED;

    delay(10);

    if (!WiFi.mode(WIFI_OFF))
        return ERROR_CMD_WIFI_DISCONECT_POWER_DOWN_WIFI_FAILED;

    return SUCCESS;
}

int CWorkingStation::CmdMqttPublish(const char *topic, const char *message)
{
    if (!m_client.connected())
        return ERROR_CMD_MQTT_PUBLISH_CLIENT_NOT_CONNECTED;

    if (m_client.publish(topic, message))
        return SUCCESS;

    return ERROR_CMD_MQTT_PUBLISH_CONN_LOST_OR_MSG_TOO_LARGE;
}