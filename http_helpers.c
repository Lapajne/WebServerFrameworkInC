/**
 * @file http_helpers.c
 * @brief Helper functions for HTTP server.
 *
 * This file implements a number of utility functions to assist in handling HTTP
 *
 * @author Alexander Lapajne
 * @date 2025-05-12
 */

#include <stdio.h>
#include <stdlib.h>

#include "http_helpers.h"


char* create_response(const char* html) {
    const char* header_template =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "\r\n";

    int content_length = (int)strlen(html);
    int header_size = snprintf(NULL, 0, header_template, content_length);
    if (header_size < 0) return NULL;

    // Allocate space for header + body
    char* response = malloc(header_size + content_length + 1);  // +1 for '\0'
    if (!response) return NULL;

    // Write the header and append the HTML body
    sprintf(response, header_template, content_length);
    memcpy(response + header_size, html, content_length);
    response[header_size + content_length] = '\0';

    return response;
}


char* get_template(char* template_name) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), "templates/%s", template_name);

    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Could not open file: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);  // +1 for null terminator
    if (!buffer) {
        printf("Memory allocation failed for file: %s\n", filepath);
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';  // Null-terminate
    fclose(file);

    return buffer;
}
