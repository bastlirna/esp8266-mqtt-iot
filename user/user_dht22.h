/*
 * user_dht22.h
 *
 *  Created on: 1. 3. 2015
 *      Author: vojta
 */

#ifndef USER_DHT22_H_
#define USER_DHT22_H_

#include "os_type.h"

void ICACHE_FLASH_ATTR user_dht22_init(void);

float ICACHE_FLASH_ATTR dht22_getTemperature();
float ICACHE_FLASH_ATTR dht22_getHumidity();

#endif /* USER_DHT22_H_ */
