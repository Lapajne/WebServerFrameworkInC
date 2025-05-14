#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "http_helpers.h"

typedef char* (*RouteHandler)(Param*, int);

typedef struct {
    const char* path;
    RouteHandler handler;
} Route;

char* create_response(const char* body);
int start_server(Route* routes);

#endif // WEB_SERVER_H