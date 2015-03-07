/*
 * user_main.c
 *
 *  Created on: 1. 3. 2015
 *      Author: vojta
 */
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"
#include "mem.h"

#include "user_network.h"
#include "user_interface.h"
#include "user_webserver.h"
#include "user_mqtt.h"
#include "user_dht22.h"

struct rst_info rtc_info;

void ICACHE_FLASH_ATTR output_redirect(const char *str) {
	os_printf("%s", str);
}

void ICACHE_FLASH_ATTR user_init() {
	// Initialize UART0
	uart_div_modify(0, UART_CLK_FREQ / 115200);

	system_rtc_mem_read(0, &rtc_info, sizeof(struct rst_info));
	if (rtc_info.flag == 1 || rtc_info.flag == 2) {
		ESP_DBG("flag = %d,epc1 = 0x%08x,epc2=0x%08x,epc3=0x%08x,excvaddr=0x%08x,depc=0x%08x,\nFatal exception (%d): \n", rtc_info.flag, rtc_info.epc1, rtc_info.epc2, rtc_info.epc3, rtc_info.excvaddr,
				rtc_info.depc, rtc_info.exccause);
	}
	struct rst_info info = { 0 };

	system_rtc_mem_write(0, &info, sizeof(struct rst_info));


	init_params();
	os_printf("Params init done.\r\n");

	init_network();
	os_printf("Network init done.\r\n");

	//user_webserver_init(80);
	//os_printf("Webserver init done.\r\n");

	init_mqtt();
	os_printf("init MQTT done.\r\n");

	user_dht22_init();
	os_printf("DHT22 init done.\r\n");



}
