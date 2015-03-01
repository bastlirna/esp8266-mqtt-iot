/*
 * user_webserver_homepage.c
 *
 *  Created on: 1. 3. 2015
 *      Author: vojta
 */

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#include "user_webserver.h"
#include "user_dht22.h"

void ICACHE_FLASH_ATTR webserver_homepage_print(struct espconn *connection) {

	webserverReplyHeader(connection, 200);

	webserverReplyS(connection, "<!DOCTYPE html><html><head><title>IoT sensor - iot.installfest.cz</title></head><body>");

	webserverReplyS(connection, "<h1>IoT sensor</h1><br/><a href=\"http://iot.installfest.cz\">iot.installfest.cz</a>");

	char buff[32];
	c_sprintf(buff, "%.1f &deg;C", dht22_getTemperature());
	webserverReplyS(connection, "<h2>Actual temperature is</h2><p>");
	webserverReplyS(connection, buff);

	c_sprintf(buff, "%.1f %%", dht22_getHumidity());
	webserverReplyS(connection, "</p><h2>Actual humidity is</h2><p>");
	webserverReplyS(connection, buff);

	webserverReplyS(connection, "</p></body></html>");

}

