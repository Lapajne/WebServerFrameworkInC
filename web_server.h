#ifndef WEB_SERVER_H
#define WEB_SERVER_H

typedef char* (*RouteHandler)(void);

typedef struct {
    const char* path;
    RouteHandler handler;
} Route;

char* create_response(const char* body);
int start_server(Route* routes);

#endif // WEB_SERVER_H