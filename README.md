# Web Server Framework

This is a simple implementation of a web server framework in C.

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
