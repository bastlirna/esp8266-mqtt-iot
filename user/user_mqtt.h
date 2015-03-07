/*
 * user_mqtt.h
 *
 *  Created on: 25. 2. 2015
 *      Author: vojta
 */

#ifndef USER_MQTT_H_
#define USER_MQTT_H_

#include "os_type.h"

void ICACHE_FLASH_ATTR init_mqtt();
void ICACHE_FLASH_ATTR user_mqtt_publish_value(float value, char *channel);
void ICACHE_FLASH_ATTR user_mqtt_connected();
void ICACHE_FLASH_ATTR user_mqtt_disconnect();

void ICACHE_FLASH_ATTR user_mqtt_publish_humidity(float value);
void ICACHE_FLASH_ATTR user_mqtt_publish_temperature(float value);

#endif /* USER_MQTT_H_ */
