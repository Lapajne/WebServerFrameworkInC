#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "web_server.h"

char* handle_index(void) {
    return strdup("<h1>Welcome to the index page!</h1>");
}

char* handle_about(void) {
    return strdup("<h1>About us</h1><p>This is the about page.</p>");
}

char* handle_contact(void) {
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
