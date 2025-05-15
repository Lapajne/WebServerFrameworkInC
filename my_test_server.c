/**
 * @file web_server_windows.c
 * @brief This file is an example implementation of an app using the web framework.
 *
 * @author Alexander Lapajne
 * @date 2025-05-12
 */


#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "web_server.h"

char* handle_index(Param* params, int param_count) {
    return strdup("<h1>Welcome to the index page!</h1>");
}

char* handle_about(Param* params, int param_count) {
    return strdup("<h1>About us</h1><p>This is the about page.</p>");
}

char* handle_contact(Param* params, int param_count) {
    return strdup("<h1>Contact</h1><p>Email us at hello@example.com</p>");
}

Route routes[] = {
    { "/", handle_index },
    { "/about", handle_about },
    { "/contact", handle_contact },
    { NULL, NULL }  // Sentinel to mark end
};


int main() {
    start_server(routes);
    return 0;
}
