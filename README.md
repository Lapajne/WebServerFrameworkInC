# Web Server Framework

This is a simple implementation of a web server framework in C.

Compile and link on a Windows system using:

    cl my_test_server.c http_helpers.c web_server_windows.c /link ws2_32.lib

The repo also contains a web application that uses the framework as an example of usage: my_test_server.c

## How to use the web server framework

Implement an array of endpoints and their corresponding function calls like this:

    Route routes[] = {
        { "/", handle_index },
        { "/about", handle_about },
        { "/contact", handle_contact },
        { NULL, NULL }
    };

Then call:

    start_server(routes)
