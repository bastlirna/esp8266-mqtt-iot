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

void user_webserver_init(uint32 port);

#endif
