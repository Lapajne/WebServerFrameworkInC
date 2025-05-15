/**
 * @file http_helpers.c
 * @brief Helper functions for HTTP server.
 *
 * This file implements a number of utility functions to assist in handling HTTP requests
 *
 * @author Alexander Lapajne
 * @date 2025-05-12
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http_helpers.h"


#ifndef _MSC_VER
#else
// Windows/MSVC does not have strndup by default
char* strndup(const char* s, size_t n) {
    char* result;
    size_t len = strlen(s);
    if (len > n)
        len = n;
    result = (char*)malloc(len + 1);
    if (!result) return NULL;
    memcpy(result, s, len);
    result[len] = '\0';
    return result;
}
#endif


char* create_response(const char* html) {
    /**
     * Create an HTTP response with the given HTML content.
     * 
     * @param html The HTML content to include in the response body.
     * @return A dynamically allocated string containing the full HTTP response.
     */

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
    /**
     * @brief Load a template file from the templates director and return the contents.
     * 
     * @param template_name The name of the template file to load (without path).
     * @return A dynamically allocated string containing the file contents, or NULL on failure.
    */

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
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}


Param* parse_params(const char* data, int* param_count_out) {
    /**
     * @brief Parse URL-encoded parameters from a query string (GET request) or POST body
     * 
     * @param data The input string containing the parameters.
     * @param param_count_out Pointer to an integer to store the number of parsed parameters.
     * @return An array of Param structs containing the parameters as key-value pairs.
    */
    *param_count_out = 0;        

    if (!data) return NULL;

    Param* params = malloc(sizeof(Param) * MAX_PARAMS);  // MAX_PARAMS is a constant you define
    int count = 0;

    const char* pos = data;
    while (*pos && count < MAX_PARAMS) {
        // Extract key
        const char* key_start = pos;
        const char* eq = strchr(pos, '=');
        if (!eq) break;

        // Extract value
        const char* val_start = eq + 1;
        const char* amp = strchr(val_start, '&');

        int key_len = (int)(eq - key_start);
        int val_len = amp ? (int)(amp - val_start) : (int)strlen(val_start);

        params[count].key = strndup(key_start, key_len);
        params[count].value = strndup(val_start, val_len);
        count++;

        pos = amp ? amp + 1 : NULL;
        if (!pos) break;
    }

    if (param_count_out) {
        *param_count_out = count;
    }

    return params;
}
