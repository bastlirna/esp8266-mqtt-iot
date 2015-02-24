/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_webserver.c
 *
 * Description: The web server mode configration.
 *              Check your hardware connection with the host while use this mode.
 * Modification history:
 *     2014/3/12, v1.0 create this file.
 *******************************************************************************/
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"

#include "version.h"

#include "espconn.h"

#include "user_webserver.h"

LOCAL char *webserver_parse_request(struct HttpRequest *request, char *p_data, unsigned short length) {
	// GET /index.html HTTP/1.1\r\n
	// Host: www.example.com\r\n
	// \r\n

	char *line = p_data;
	uint16_t line_n = 0;
	uint16_t len = 0;
	do {

		if (line == 0) {

			if ((char *)os_strstr(line, "GET") == line) {
				// this is GET request
				request->type = GET;
			} else if ((char *)os_strstr(line, "POST") == line) {
				// this is POST request
				request->type = POST;
			} else if ((char *)os_strstr(line, "HEAD") == line) {
				// this is HEAD request
				request->type = HEAD;
			}

			char *r_path = (char *)os_strstr(line, " ");
			char *r_version = (char *)os_strstr(r_path, " ");

			switch (request->type) {
			case GET:
			case HEAD:
				// TODO: extract parameters

				len = (uint16_t) (r_version- r_path);
				if(len > MAX_HTTP_PATH_LEN - 1){
					len = MAX_HTTP_PATH_LEN - 1;
				}
				os_strncpy(request->path, r_path, len);

				break;
			}

		}

		line = (char *)os_strstr(line, "\r\n") + 2;
		line_n++;
	} while (line);

	return p_data;
}
#define SERVER_SSL_ENABLE
LOCAL void ICACHE_FLASH_ATTR webserver_reply(struct espconn *connection) {
	char httphead[256];

	os_sprintf(httphead, "HTTP/1.0 200 OK\r\nServer: ESPuWeb 1.0\r\n\r\nHello");

#ifdef SERVER_SSL_ENABLE
	espconn_secure_sent(connection, httphead, os_strlen(httphead));
#else
	espconn_sent(connection, httphead, os_strlen(httphead));
#endif
}

LOCAL void ICACHE_FLASH_ATTR webserver_recv(void *arg, char *p_data, unsigned short length) {
	struct espconn *ptrespconn = arg;

	struct HttpRequest req;

	webserver_parse_request(&req, p_data, length);
	// TODO: parse request

	webserver_reply(ptrespconn);

}

LOCAL ICACHE_FLASH_ATTR void webserver_recon(void *arg, sint8 err) {
	struct espconn *pesp_conn = arg;
}

LOCAL ICACHE_FLASH_ATTR void webserver_discon(void *arg) {
	struct espconn *pesp_conn = arg;
}

LOCAL void ICACHE_FLASH_ATTR webserver_listen(void *arg) {
	struct espconn *pesp_conn = arg;

	espconn_regist_recvcb(pesp_conn, webserver_recv);
	espconn_regist_reconcb(pesp_conn, webserver_recon);
	espconn_regist_disconcb(pesp_conn, webserver_discon);
}

void ICACHE_FLASH_ATTR user_webserver_init(uint32 port) {
	LOCAL struct espconn esp_conn;
	LOCAL esp_tcp esptcp;

	esp_conn.type = ESPCONN_TCP;
	esp_conn.state = ESPCONN_NONE;
	esp_conn.proto.tcp = &esptcp;
	esp_conn.proto.tcp->local_port = port;
	espconn_regist_connectcb(&esp_conn, webserver_listen);

//#define SERVER_SSL_ENABLE
#ifdef SERVER_SSL_ENABLE
	espconn_secure_accept(&esp_conn);
#else
	espconn_accept(&esp_conn);
#endif
}
