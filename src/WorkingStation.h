#pragma once

#include <WString.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <FS.h>
#include <Wire.h>

#include <PubSubClient.h>
#include "GlobalDefinitions.h"

#include <queue>
#include <vector>
#include <map>

class CWorkingStation
{
public:
	CWorkingStation();

	virtual ~CWorkingStation()
	{
		if (m_ssid)
			delete[] m_ssid;

		if (m_psk)
			delete[] m_psk;
	};

	void HandleCommands();

private:
	bool InitFromFS();

	void FetchCommands();
	int ExecuteCommand(int* outCmdId);
	bool HasCommands();

	int ConnectToMQTT();
	
	int ConnectToWifi();
	int DisconnectFromWifi();

	int CmdMqttPublish(const char *topic, const char *message);

	bool ExecuteCommandOld();

private:
	enum MESSAGE_PARSER
	{
		WAIT_START = 0,
		WAIT_END = 1,
		PARSE_MESSAGE = 2,
	};

	struct tzCOMMAND
	{
		int m_id;
		String m_command;
		std::map<String, String> m_args;
	};

private:
	char *m_ssid;
	char *m_psk;

	IPAddress m_mqttServerIPAddr;
	int m_port;

	PubSubClient m_client;
	WiFiClient m_espClient;

	std::queue<tzCOMMAND *> m_commandQueue;

	// Parser temp vars.
	MESSAGE_PARSER m_currState;
	String m_messageBuffer;
};