#ifndef __USER_WEBSERVER_H__
#define __USER_WEBSERVER_H__

#define SERVER_PORT 80
#define SERVER_SSL_PORT 443

typedef enum {
    HTTP_GET = 0,
    HTTP_POST,
    HTTP_HEAD,
    HTTP_UNDEFINED
} RequestType;


#define MAX_HTTP_PATH_LEN 32

typedef enum {
	ContentTypeNone = 0,
	ApplicationXWwwFormUrlencoded,
	MultipartFormData
} ContentType;

struct HttpRequest{
	RequestType type;
	ContentType contentType;
	uint32_t requestSize;
	char hostname[MAX_HTTP_PATH_LEN];
	char path[MAX_HTTP_PATH_LEN];
	//HttpRequestVariables *variables;
};
/*
typedef struct _HttpRequestVariables HttpRequestVariables;
struct _HttpRequestVariables{
	char *name;
	char *value;
	HttpRequestVariables *next;
};*/

#define MAX_REQUEST_LINE_LEN 100

void ICACHE_FLASH_ATTR webserverReplyS(struct espconn *connection, char *str);
void ICACHE_FLASH_ATTR webserverReply(struct espconn *connection, uint8 *psent, uint16 length);
void ICACHE_FLASH_ATTR webserverReplyHeader(struct espconn *connection, uint16_t code);
void ICACHE_FLASH_ATTR user_webserver_init(uint32 port);

void ICACHE_FLASH_ATTR webserver_homepage_print(struct espconn *connection);

#endif
