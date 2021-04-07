## TODO
- ✔️ MQTT connect command does not return a result
- ✔️ Q:After 1 MQTT publish, a new connection needs to be made. Investigate why
      A: PubSubClient* setSocketTimeout (timeout). By default, it is set to 15 seconds - as defined by the MQTT_SOCKET_TIMEOUT constant in PubSubClient.h.

## LEGEND
✔️ - DONE
❌ - NOT STARTED
❔ - STILL UNKNOWN / IN PROGRESS