#ifndef __APE_SERVER_H
#define __APE_SERVER_H

#include "common.h"
#include "socket.h"
#include "http_parser.h"
#include "JSON_parser.h"
#include "ape_buffer.h"
#include "ape_array.h"
#include "ape_transports.h"


#define APE_CLIENT(socket) ((ape_client *)socket->ctx)


typedef struct {
	ape_socket *socket;
	char ip[16];
	struct {
		http_parser parser;
		http_method_t method;
		ape_transport_t transport;
		buffer *path;
		buffer *qs;
		struct {
			ape_array_t *list;
			buffer *tkey;
			buffer *tval;
		} headers;
	} http;
	
	struct {
		struct JSON_parser_struct* parser;
	} json;
	
} ape_client;

typedef struct {
	char ip[16];
	ape_array_t *hosts;
	ape_socket *socket;  /* socket of the server   */
	uint16_t port;
	uint32_t nconnected; /* number of fd connected */
} ape_server;

ape_server *ape_server_init(uint16_t port, const char *local_ip, ape_global *ape);

#endif
