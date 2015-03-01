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

	wifi_station_get_config(&stationConf);

	os_strcpy(stationConf.ssid, get_params()->ssid);
	if(*get_params()->pass){
		os_strcpy(stationConf.password, get_params()->pass);
	}else{
		os_strcpy(stationConf.password, "");
	}

	wifi_station_disconnect();
	wifi_station_dhcpc_stop();

	if(!wifi_station_set_config(&stationConf))
	{
		// CTRL not set station config!
	}

	wifi_station_connect();
	wifi_station_dhcpc_start();
	wifi_station_set_auto_connect(1);
	if(wifi_get_phy_mode() != PHY_MODE_11N)
		wifi_set_phy_mode(PHY_MODE_11N);
	if(wifi_station_get_auto_connect() == 0)
		wifi_station_set_auto_connect(1);

	wifi_station_get_config(&stationConf);

	//wifi_station_get_connect_status();
	// STATION_GOT_IP, STATION_WRONG_PASSWORD, STATION_NO_AP_FOUND, STATION_CONNECT_FAIL
}

