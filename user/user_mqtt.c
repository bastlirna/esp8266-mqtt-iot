/*
 * user_mqtt.c
 *
 *  Created on: 25. 2. 2015
 *      Author: vojta
 */

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"
#include "mem.h"

#include "user_network.h"
#include "user_interface.h"

#include "mqtt.h"

MQTT_Client mqttClient;

LOCAL char macaddr[6];

LOCAL bool connected = false;
LOCAL bool initialized = false;


void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args)
{

	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Connected\r\n");
	//MQTT_Subscribe(client, "/mqtt/topic/0", 0);
	//MQTT_Subscribe(client, "/mqtt/topic/1", 1);
	//MQTT_Subscribe(client, "/mqtt/topic/2", 2);


	//MQTT_Publish(client, "/MG_IOT/test", "iamonline", 6, 1, 0);
	//MQTT_Publish(client, "/mqtt/topic/2", "hello2", 6, 2, 0);

	connected = true;
}
/*
void ICACHE_FLASH_ATTR user_mqtt_publish_humidity(float value){
	user_mqtt_publish_value(value, "humidity");
}

void ICACHE_FLASH_ATTR user_mqtt_publish_temperature(float value){
	user_mqtt_publish_value(value, "temperature");
}
*/
void ICACHE_FLASH_ATTR user_mqtt_publish_value(char *s_value, char *channel){

	static char path[128];

	if(!connected){
		TRACE("[mqtt] is not connected!\r\n");
		return;
	}

	os_sprintf(path, "/sh/mucirna/%02X%02X%02X%02X%02X%02X/%s", MAC2STR(macaddr), channel);
	TRACE("[mqtt] publish: %s => %s\r\n", path, s_value);
	MQTT_Publish(&mqttClient, path, s_value, os_strlen(s_value), 0, 0);
	TRACE("[mqtt] done\r\n");

}

void ICACHE_FLASH_ATTR user_mqtt_publish_valueF(float value, char *channel){
	static char s_value[32];
	c_sprintf(s_value, "%.1f", value);
	user_mqtt_publish_value(s_value, channel);
}

void ICACHE_FLASH_ATTR user_mqtt_publish_valueU(uint16_t value, char *channel){
	static char s_value[32];
	c_sprintf(s_value, "%u", value);
	user_mqtt_publish_value(s_value, channel);
}


void ICACHE_FLASH_ATTR mqttDisconnectedCb(uint32_t *args)
{
	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Disconnected\r\n");
	connected = false;
}

void ICACHE_FLASH_ATTR mqttPublishedCb(uint32_t *args)
{
	MQTT_Client* client = (MQTT_Client*)args;
	INFO("MQTT: Published\r\n");
}

void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
	char *topicBuf = (char*)os_zalloc(topic_len+1),
			*dataBuf = (char*)os_zalloc(data_len+1);

	MQTT_Client* client = (MQTT_Client*)args;

	//os_memcpy(topicBuf, topic, topic_len);
	//topicBuf[topic_len] = 0;

	//os_memcpy(dataBuf, data, data_len);
	//dataBuf[data_len] = 0;

	INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);
	os_free(topicBuf);
	os_free(dataBuf);
}

void ICACHE_FLASH_ATTR user_mqtt_connect(){

	if(!connected && initialized){
		INFO("MQTT: Connnect\r\n");
		MQTT_Connect(&mqttClient);
	}

}

void ICACHE_FLASH_ATTR user_mqtt_disconnect(){

	if(connected && initialized){
		INFO("MQTT: Disconnnect\r\n");
		MQTT_Disconnect(&mqttClient);
	}

}

void ICACHE_FLASH_ATTR init_mqtt(){

	char ident[64];

	MQTT_InitConnection(&mqttClient, "147.32.30.132", 1883, 0);


	wifi_get_macaddr(SOFTAP_IF, macaddr);

	os_sprintf(ident, "MacGyver-IoT_%02x%02x%02x%02x%02x%02x", MAC2STR(macaddr));
	MQTT_InitClient(&mqttClient, ident, "", "", 120, 1);

	//MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);

	initialized = true;
/*
	MQTT_Connect(&mqttClient);
		} else {
			MQTT_Disconnect(&mqttClient);
*/
}
