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
    uint8 ssid[40];
    uint8 pass[40];
};

struct esp_platform_sec_flag_param {
    uint8 flag;
    uint8 pad[3];
};


void ICACHE_FLASH_ATTR user_esp_platform_save_param(struct esp_platform_saved_param *param);
void ICACHE_FLASH_ATTR user_esp_platform_load_param(struct esp_platform_saved_param *param);

#endif /* USER_PARAMS_H_ */
