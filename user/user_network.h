/*
 * user_network.h
 *
 *  Created on: 22. 2. 2015
 *      Author: vojta
 */

#ifndef USER_NETWORK_H_
#define USER_NETWORK_H_

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"

void ICACHE_FLASH_ATTR init_network();
void ICACHE_FLASH_ATTR setup_station_mode();

enum NetworkConnectionState{
	USER_AP_MODE,
	USER_STATION_MODE

};

typedef void (*ConnectionStateCallback)(uint8_t);

#endif /* USER_NETWORK_H_ */
