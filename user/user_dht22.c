/*
 * user_dht22.c
 *
 *  Created on: 1. 3. 2015
 *  	Author: Vojta
 *      Based on: https://github.com/fasmide/esp_dht22
 */
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "gpio.h"


#define MAXTIMINGS 10000
#define BREAKTIME 20

static volatile os_timer_t some_timer;
static float lastTemp, lastHum;



static void ICACHE_FLASH_ATTR readDHT(void *arg) {
	int counter = 0;
	int laststate = 1;
	int i = 0;
	int j = 0;
	int checksum = 0;

	DEBUG("[DHT22] start read \r\n");

	int data[100];
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	GPIO_OUTPUT_SET(2, 1);
	os_delay_us(250000);

	GPIO_OUTPUT_SET(2, 0);
	os_delay_us(2000);

	GPIO_OUTPUT_SET(2, 1);
	os_delay_us(40);
	GPIO_DIS_OUTPUT(2);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);

	// wait for pin to drop?
	while (GPIO_INPUT_GET(2) == 1 && i < 100000) {
		os_delay_us(1);
		i++;
	}
	if (i == 100000)
		return;
	// read data!
	for (i = 0; i < MAXTIMINGS; i++) {
		counter = 0;
		while (GPIO_INPUT_GET(2) == laststate) {
			counter++;
			os_delay_us(1);
			if (counter == 1000)
				break;
		}
		laststate = GPIO_INPUT_GET(2);
		if (counter == 1000)
			break;
		//bits[bitidx++] = counter;
		if ((i > 3) && (i % 2 == 0)) {
			// shove each bit into the storage bytes
			data[j / 8] <<= 1;
			if (counter > BREAKTIME)
				data[j / 8] |= 1;
			j++;
		}
	}
	/*
	 for (i=3; i<bitidx; i+=2) {
	 os_printf("bit %d: %d\n", i-3, bits[i]);
	 os_printf("bit %d: %d (%d)\n", i-2, bits[i+1], bits[i+1] > BREAKTIME);
	 }
	 os_printf("Data (%d): 0x%x 0x%x 0x%x 0x%x 0x%x\n", j, data[0], data[1], data[2], data[3], data[4]);
	 */
	float temp_p, hum_p;
	if (j >= 39) {
		checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
		if (data[4] == checksum) {
			/* yay! checksum is valid */
			hum_p = data[0] * 256 + data[1];
			hum_p /= 10;
			temp_p = (data[2] & 0x7F) * 256 + data[3];
			temp_p /= 10.0;
			if (data[2] & 0x80)
				temp_p *= -1;
			//sendReading(temp_p, hum_p);
			char tmp[30];

			lastTemp = temp_p;
			lastHum = hum_p;

			DEBUG("[DHT22] Temp = %.1f, Hum = %.1f\r\n", temp_p, hum_p);
		}
	}
	DEBUG("[DHT22] end read \r\n");


}
void ICACHE_FLASH_ATTR user_dht22_init(void) {

	//Set GPIO2 to output mode
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);

	os_timer_disarm(&some_timer);
	//Setup timer
	os_timer_setfn(&some_timer, (os_timer_func_t *) readDHT, NULL);

	//Arm the timer
	//&some_timer is the pointer
	//1000 is the fire time in ms
	//0 for once and 1 for repeating
	os_timer_arm(&some_timer, 2500, 1);
}


