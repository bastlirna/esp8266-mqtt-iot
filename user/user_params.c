/*
 * user_params.c
 *
 *  Created on: 22. 2. 2015
 *      Author: vojta
 */

#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_config.h"

#include "user_params.h"

struct esp_platform_saved_param esp_param;

struct esp_platform_saved_param ICACHE_FLASH_ATTR *get_params()
{
	return &esp_param;
}

LOCAL void ICACHE_FLASH_ATTR user_esp_platform_load_param(struct esp_platform_saved_param *param) {
	struct esp_platform_sec_flag_param flag;

	spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE, (uint32 *) &flag, sizeof(struct esp_platform_sec_flag_param));

	if (flag.flag == 0) {
		spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_0) * SPI_FLASH_SEC_SIZE, (uint32 *) param, sizeof(struct esp_platform_saved_param));
	} else {
		spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_1) * SPI_FLASH_SEC_SIZE, (uint32 *) param, sizeof(struct esp_platform_saved_param));
	}
}

LOCAL void ICACHE_FLASH_ATTR user_esp_platform_save_param(struct esp_platform_saved_param *param) {
	struct esp_platform_sec_flag_param flag;

	spi_flash_read((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE, (uint32 *) &flag, sizeof(struct esp_platform_sec_flag_param));

	if (flag.flag == 0) {
		spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_SAVE_1);
		spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_1) * SPI_FLASH_SEC_SIZE, (uint32 *) param, sizeof(struct esp_platform_saved_param));
		flag.flag = 1;
		spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_FLAG);
		spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE, (uint32 *) &flag, sizeof(struct esp_platform_sec_flag_param));
	} else {
		spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_SAVE_0);
		spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_SAVE_0) * SPI_FLASH_SEC_SIZE, (uint32 *) param, sizeof(struct esp_platform_saved_param));
		flag.flag = 0;
		spi_flash_erase_sector(ESP_PARAM_START_SEC + ESP_PARAM_FLAG);
		spi_flash_write((ESP_PARAM_START_SEC + ESP_PARAM_FLAG) * SPI_FLASH_SEC_SIZE, (uint32 *) &flag, sizeof(struct esp_platform_sec_flag_param));
	}
}

void ICACHE_FLASH_ATTR init_params(){
	user_esp_platform_load_param(&esp_param);

	char ver[32];
	c_sprintf(ver, "SIZE:%u", sizeof(esp_param));

	if(os_strncmp(esp_param.version, ver, strlen(ver))){
		os_memset(&esp_param, 0, sizeof(esp_param));
		os_strcpy(esp_param.version, ver);

		params_save();
	}

	char *essid = "SH_IF";
	char *pass = "InstallFest15";
	os_strcpy(esp_param.ssid, essid, os_strlen(essid));
	os_strcpy(esp_param.pass, pass, os_strlen(pass));

}


void ICACHE_FLASH_ATTR params_save(){
	user_esp_platform_save_param(&esp_param);
}

