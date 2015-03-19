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

void ICACHE_FLASH_ATTR user_mqtt_publish_valueF(float value, char *channel);
void ICACHE_FLASH_ATTR user_mqtt_publish_valueU(uint16_t value, char *channel);
void ICACHE_FLASH_ATTR user_mqtt_publish_value(char *s_value, char *channel);
void ICACHE_FLASH_ATTR user_mqtt_connect();
void ICACHE_FLASH_ATTR user_mqtt_disconnect();



#endif /* USER_MQTT_H_ */
