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
#include "user_network.h"
#include "user_mqtt.h"


LOCAL ETSTimer wiFiLinker;

LOCAL void ICACHE_FLASH_ATTR setup_ap_mode();

static lastWifiStatus = STATION_IDLE;

uint8_t retries = 0;

enum NetworkConnectionState state = 0;

LOCAL void ICACHE_FLASH_ATTR station_connect() {
#ifndef INSTALLFEST
	retries++;

	if(retries > 1){
		state = USER_AP_MODE;

		DEBUG("[NETWORK] Network is not available,  going to run as AP\r\n");

		setup_ap_mode();
		return;
	}
#endif
	wifi_station_connect();
}

LOCAL void ICACHE_FLASH_ATTR wifi_check_ip(void *arg) {
	struct ip_info ipConfig;
	uint8_t wifiStatus = STATION_IDLE;

	//INFO("[NETWORK]: Check\r\n");

	os_timer_disarm(&wiFiLinker);

	wifi_get_ip_info(STATION_IF, &ipConfig);

	wifiStatus = wifi_station_get_connect_status();

	if (wifiStatus == STATION_GOT_IP && ipConfig.ip.addr != 0) {

		os_timer_setfn(&wiFiLinker, (os_timer_func_t *) wifi_check_ip, NULL);
		os_timer_arm(&wiFiLinker, 2000, 0);

	} else {

		if (wifi_station_get_connect_status() == STATION_WRONG_PASSWORD) {
			station_connect();
		} else if (wifi_station_get_connect_status() == STATION_NO_AP_FOUND) {
			station_connect();
		} else if (wifi_station_get_connect_status() == STATION_CONNECT_FAIL) {
			station_connect();
		} else {
			//DEBUG("[NETWORK] error: STATION_IDLE\r\n");
		}

		if(state == USER_STATION_MODE){
			os_timer_setfn(&wiFiLinker, (os_timer_func_t *) wifi_check_ip, NULL);
			os_timer_arm(&wiFiLinker, 500, 0);
		}

	}

	if (wifiStatus != lastWifiStatus) {
		TRACE("[NETWORK] wifiStatus: %u\r\n", wifiStatus);
		if(wifiStatus == STATION_GOT_IP){

			DEBUG("[NETWORK] Connected to network (IP: "IPSTR")\r\n", IP2STR(&ipConfig.ip));

			user_mqtt_connected();
		} else {

			if (wifi_station_get_connect_status() == STATION_WRONG_PASSWORD) {
				DEBUG("[NETWORK] error: STATION_WRONG_PASSWORD\r\n");
			} else if (wifi_station_get_connect_status() == STATION_NO_AP_FOUND) {
				DEBUG("[NETWORK] error: STATION_NO_AP_FOUND\r\n");
			} else if (wifi_station_get_connect_status() == STATION_CONNECT_FAIL) {
				DEBUG("[NETWORK] error: STATION_CONNECT_FAIL\r\n");
			}

			user_mqtt_disconnect();
		}

		lastWifiStatus = wifiStatus;


	}
	INFO("[NETWORK]: Check done\r\n");
}

LOCAL void ICACHE_FLASH_ATTR runCheckTimer() {
	os_timer_disarm(&wiFiLinker);
	os_timer_setfn(&wiFiLinker, (os_timer_func_t *) wifi_check_ip, NULL);
	os_timer_arm(&wiFiLinker, 1000, 0);
}

void ICACHE_FLASH_ATTR init_network() {

	if (os_strlen(get_params()->ssid) > 0) {

		state = USER_STATION_MODE;

		DEBUG("[NETWORK] SSID is not null, trying connect\r\n");

		retries = 0;
		setup_station_mode();

		runCheckTimer();

	} else {
#ifndef INSTALLFEST
		state = USER_AP_MODE;

		DEBUG("[NETWORK] SSID is null, creating AP\r\n");
		setup_ap_mode();
#endif
	}
}

LOCAL void ICACHE_FLASH_ATTR setup_ap_mode() {
	wifi_station_disconnect();
	wifi_station_dhcpc_stop();

	wifi_set_opmode(SOFTAP_MODE);

	struct softap_config apconfig;

	if (wifi_softap_get_config(&apconfig)) {
		wifi_softap_dhcps_stop();
		char macaddr[6];
		wifi_get_macaddr(SOFTAP_IF, macaddr);

		os_memset(apconfig.ssid, 0, sizeof(apconfig.ssid));
		os_memset(apconfig.password, 0, sizeof(apconfig.password));
		apconfig.ssid_len = os_sprintf(apconfig.ssid, "MacGyver-IoT_%02x%02x%02x%02x%02x%02x", MAC2STR(macaddr));
		//os_sprintf(apconfig.password, "%02x%02x%02x%02x%02x%02x", MAC2STR(macaddr)); // 18fe349bc6b6
		//os_sprintf(apconfig.password, "test"); // 18fe349bc6b6
		//apconfig.authmode = AUTH_WPA_WPA2_PSK; // AUTH_OPEN
		apconfig.authmode = AUTH_OPEN;
		apconfig.ssid_hidden = 0;
		apconfig.channel = 7;
		apconfig.max_connection = 10;

		if (!wifi_softap_set_config(&apconfig)) {
			// CTRL not set AP config!
		}

		struct ip_info ipinfo;
		if (wifi_get_ip_info(SOFTAP_IF, &ipinfo)) {
			IP4_ADDR(&ipinfo.ip, 192, 168, 4, 1);
			IP4_ADDR(&ipinfo.gw, 192, 168, 4, 1);
			IP4_ADDR(&ipinfo.netmask, 255, 255, 255, 0);
			if (!wifi_set_ip_info(SOFTAP_IF, &ipinfo)) {
				// CTRL not set IP config!
			}
		}

		wifi_softap_dhcps_start();

	}

	if (wifi_get_phy_mode() != PHY_MODE_11N)
		wifi_set_phy_mode(PHY_MODE_11N);

}

