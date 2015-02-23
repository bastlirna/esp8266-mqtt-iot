/*
 * user_network.c
 *
 *  Created on: 22. 2. 2015
 *      Author: vojta
 */

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"
#include "user_interface.h"

#include "user_params.h"

LOCAL struct esp_platform_saved_param esp_param;

static void ICACHE_FLASH_ATTR setup_ap_mode();
static void ICACHE_FLASH_ATTR setup_station_mode();


void ICACHE_FLASH_ATTR init_network()
{

	if(*esp_param.ssid != 0){

		setup_station_mode();

		while(wifi_station_get_connect_status() == STATION_CONNECTING)
		{
			os_delay_us(200);
		}

		if(wifi_station_get_connect_status() != STATION_GOT_IP)
		{
			setup_ap_mode();
		}
	}
	else
	{
		setup_ap_mode();
	}

}

static void ICACHE_FLASH_ATTR setup_ap_mode()
{
	wifi_station_disconnect();
	wifi_station_dhcpc_stop();

	wifi_set_opmode(SOFTAP_MODE);

	struct softap_config apconfig;

	if(wifi_softap_get_config(&apconfig))
	{
		wifi_softap_dhcps_stop();
		char macaddr[6];
		wifi_get_macaddr(SOFTAP_IF, macaddr);

		os_memset(apconfig.ssid, 0, sizeof(apconfig.ssid));
		os_memset(apconfig.password, 0, sizeof(apconfig.password));
		apconfig.ssid_len = os_sprintf(apconfig.ssid, "MacGyverIoT_%02x%02x%02x%02x%02x%02x", MAC2STR(macaddr));
		os_sprintf(apconfig.password, "%02x%02x%02x%02x%02x%02x", MAC2STR(macaddr));
		apconfig.authmode = AUTH_WPA_WPA2_PSK; // AUTH_OPEN
		apconfig.ssid_hidden = 0;
		apconfig.channel = 7;
		apconfig.max_connection = 10;

		if(!wifi_softap_set_config(&apconfig))
		{
			// CTRL not set AP config!
		}

		struct ip_info ipinfo;
		if(wifi_get_ip_info(SOFTAP_IF, &ipinfo))
		{
			IP4_ADDR(&ipinfo.ip, 192, 168, 4, 1);
			IP4_ADDR(&ipinfo.gw, 192, 168, 4, 1);
			IP4_ADDR(&ipinfo.netmask, 255, 255, 255, 0);
			if(!wifi_set_ip_info(SOFTAP_IF, &ipinfo))
			{
				// CTRL not set IP config!
			}
		}

		wifi_softap_dhcps_start();

	}

	if(wifi_get_phy_mode() != PHY_MODE_11N)
		wifi_set_phy_mode(PHY_MODE_11N);
	if(wifi_station_get_auto_connect() == 0)
		wifi_station_set_auto_connect(1);

}

static void ICACHE_FLASH_ATTR setup_station_mode()
{
	struct station_config stationConf;

	wifi_set_opmode(STATION_MODE);

	wifi_station_get_config(&stationConf);

	os_strcpy(stationConf.ssid, esp_param.ssid);
	if(*esp_param.pass){
		os_strcpy(stationConf.password, esp_param.pass);
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



