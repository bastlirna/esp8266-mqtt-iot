/*
 * user_network_station.c
 *
 *  Created on: 25. 2. 2015
 *      Author: vojta
 */

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"

#include "user_params.h"

void ICACHE_FLASH_ATTR setup_station_mode()
{
	struct station_config stationConf;

	wifi_set_opmode(STATION_MODE);
	wifi_station_set_auto_connect(FALSE);

	os_memset(&stationConf, 0, sizeof(stationConf));

	os_strcpy(stationConf.ssid, get_params()->ssid);
	if(*get_params()->pass){
		os_strcpy(stationConf.password, get_params()->pass);
	}else{
		os_strcpy(stationConf.password, "");
	}


	if(!wifi_station_set_config(&stationConf))
	{
		// CTRL not set station config!
		INFO("[CTRL] not set station config");
	}

	wifi_station_set_auto_connect(TRUE);
	wifi_station_connect();


	TRACE("[NETWORK] Client mode setting done\r\n");
	//wifi_station_get_connect_status();
	// STATION_GOT_IP, STATION_WRONG_PASSWORD, STATION_NO_AP_FOUND, STATION_CONNECT_FAIL
}

