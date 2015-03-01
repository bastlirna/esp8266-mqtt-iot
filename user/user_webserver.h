#ifndef __USER_WEBSERVER_H__
#define __USER_WEBSERVER_H__

#define SERVER_PORT 80
#define SERVER_SSL_PORT 443

typedef enum RequestType {
    GET = 0,
    POST,
    HEAD,
} ProtocolType;


#define MAX_HTTP_PATH_LEN 32

struct HttpRequest{
	ProtocolType type;
	char path[MAX_HTTP_PATH_LEN];

};

void ICACHE_FLASH_ATTR webserverReplyS(struct espconn *connection, char *str);
void ICACHE_FLASH_ATTR webserverReply(struct espconn *connection, uint8 *psent, uint16 length);
void ICACHE_FLASH_ATTR webserverReplyHeader(struct espconn *connection, uint16_t code);
void ICACHE_FLASH_ATTR user_webserver_init(uint32 port);

void ICACHE_FLASH_ATTR webserver_homepage_print(struct espconn *connection);

#endif
