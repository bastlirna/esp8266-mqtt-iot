/*
 * user_webserver.c
 *
 *  Created on: 1. 3. 2015
 *      Author: vojta
 */
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "version.h"
#include "espconn.h"

#include "user_webserver.h"

#define charAtPosition(l,p) (*((char *)((uint32_t)l + p)))
#define toUpper(a) ((a > 'A' && a < 'Z') ? a : 'a' - 'A' + a)

LOCAL uint8_t ICACHE_FLASH_ATTR nibbleToInt(char *line, uint8_t pos){

	if((charAtPosition(line, pos) < '9') && (charAtPosition(line, pos) > '0')){
		return charAtPosition(line, pos) - '0';
	}
	return toUpper(charAtPosition(line, pos)) - 'A' + 10;
}


LOCAL void ICACHE_FLASH_ATTR webserver_url_decode(char *data, uint32 len){

	char *output = data;
	while(len > 0){
		char ch = *data;
		if(*data == '%' && len > 2){
			ch = (nibbleToInt(data, 1) << 4) | (nibbleToInt(data, 2));
			data+=2;
			len-=2;
		}
		*output = ch;

		output++;
		data++;
		len--;
	}
	*output = 0;

}

LOCAL void ICACHE_FLASH_ATTR webserver_parse_method_type(struct HttpRequest *request, char *line){

	request->type = HTTP_UNDEFINED;
	if ((char *)os_strstr(line, "GET") == line) {
		// this is GET request
		request->type = HTTP_GET;
	} else if ((char *)os_strstr(line, "POST") == line) {
		// this is POST request
		request->type = HTTP_POST;
	} else if ((char *)os_strstr(line, "HEAD") == line) {
		// this is HEAD request
		request->type = HTTP_HEAD;
	}

	char *r_path = (char *)os_strstr(line, " ") + 1;
	char *r_version = (char *)os_strstr(r_path, " ") + 1;
	uint16_t len = 0;

	switch (request->type) {
	case HTTP_GET:
	case HTTP_HEAD:
	case HTTP_POST:
		// TODO: extract parameters

		len = (uint16_t) (r_version - r_path);
		if(len > MAX_HTTP_PATH_LEN - 1){
			len = MAX_HTTP_PATH_LEN - 1;
		}
		os_strncpy(request->path, r_path, len);

		DEBUG("[WEBSERVER] GET request (%u:%s)\r\n", len, request->path);

		break;
	}
}

LOCAL void ICACHE_FLASH_ATTR webserver_parse_header_param(struct HttpRequest *request, char *line){

	char *valuePos = (char *)os_strstr(line, ":") + 1;

	while(*valuePos == ' ');

	if(valuePos != NULL){

		if(os_strstr(line, "Content-Length")){
			request->type = ContentTypeNone;
			if(os_strstr(valuePos, "application/x-www-form-urlencoded")){
				request->type = ApplicationXWwwFormUrlencoded;
			}else if(os_strstr(valuePos, "multipart/form-data")){
				request->type = MultipartFormData;
			}
		}else if(os_strstr(line, "Content-Length")){
			request->requestSize = atoi(valuePos);
		}else if(os_strstr(line, "Host")){
			char *endOfValue = (char *)os_strstr(valuePos, "\r\n");
			if(endOfValue != NULL){
				os_strncpy(request->hostname, valuePos, endOfValue - valuePos);
			}
		}

	}

}

LOCAL char ICACHE_FLASH_ATTR *webserver_parse_header(struct HttpRequest *request, char *p_data, unsigned short length) {

	if(p_data == NULL || length == 0){
		DEBUG("[WEBSERVER] request is null\r\n");
		return NULL;
	}

	char *line = p_data;
	uint16_t line_n = 0;
	uint16_t len = 0;

	char *headerEnd = (char *)os_strstr(line, "\r\n\r\n");

	if(headerEnd == NULL){
		DEBUG("[WEBSERVER] invalid request\r\n");
		return NULL;
	}

	do {

		char *endOfLine = (char *)os_strstr(line, "\r\n");

		if (line_n == 0) {
			webserver_parse_method_type(request, line);
		}else{
			webserver_parse_header_param(request, line);
		}

		line = endOfLine + 2;
		line_n++;
	} while (line < headerEnd);

	if(request->type == HTTP_GET || request->type == HTTP_HEAD){
		char *dataPtr = (char *)os_strstr("?", request->path);
		if(dataPtr != NULL){

			char *param = dataPtr + 1;
			// TODO: parse params

		}
	}


	return p_data;
}


void ICACHE_FLASH_ATTR webserverReply(struct espconn *connection, uint8 *psent, uint16 length) {

#ifdef SERVER_SSL_ENABLE
	espconn_secure_sent(connection, psent, length);
#else
	espconn_sent(connection, psent, length);
#endif
}

void ICACHE_FLASH_ATTR webserverReplyS(struct espconn *connection, char *str) {
	webserverReply(connection, str, os_strlen(str));
}

void ICACHE_FLASH_ATTR webserverReplyHeader(struct espconn *connection, uint16_t code) {
	char httphead[256];

	c_sprintf(httphead, "HTTP/1.0 %u OK\r\nServer: ESPuWeb 1.0\r\n\r\n", code);

	webserverReply(connection, httphead, os_strlen(httphead));

}

LOCAL void ICACHE_FLASH_ATTR webserver_recv(void *arg, char *p_data, unsigned short length) {
	struct espconn *connection = arg;

	struct HttpRequest req;
	os_memset(&req, 0, sizeof(req));

	webserver_parse_header(&req, p_data, length);

	DEBUG("[WEBSERVER] request: %u path: %s\r\n", req.type, req.path);


	// TODO: parse request

	webserver_homepage_print(connection);

	espconn_disconnect(connection);
}

LOCAL void ICACHE_FLASH_ATTR webserver_recon(void *arg, sint8 err) {
	struct espconn *pesp_conn = arg;
}

LOCAL void ICACHE_FLASH_ATTR webserver_discon(void *arg) {
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
