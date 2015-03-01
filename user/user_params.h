/*
 * user_params.h
 *
 *  Created on: 22. 2. 2015
 *      Author: vojta
 */

#ifndef USER_PARAMS_H_
#define USER_PARAMS_H_

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"

/* NOTICE---this is for 512KB spi flash.
 * you can change to other sector if you use other size spi flash. */
#define ESP_PARAM_START_SEC		0x3C

#define SPI_FLASH_SEC_SIZE      4096

#define ESP_PARAM_SAVE_0    1
#define ESP_PARAM_SAVE_1    2
#define ESP_PARAM_FLAG      3

struct esp_platform_saved_param {
	uint8 version[32];
    uint8 ssid[40];
    uint8 pass[40];
};

struct esp_platform_sec_flag_param {
    uint8 flag;
    uint8 pad[3];
};

void params_save();
void init_params();
struct esp_platform_saved_param *get_params();

#endif /* USER_PARAMS_H_ */
