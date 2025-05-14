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
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}


QueryParams parse_params(const char* data, int* param_count_out) {
    QueryParams* params = malloc(sizeof(Param) * MAX_PARAMS);  // MAX_PARAMS is a constant you define
    int count = 0;

    const char* pos = data;
    while (*pos && count < MAX_PARAMS) {
        // Extract key
        const char* key_start = pos;
        const char* eq = strchr(pos, '=');
        if (!eq) break;

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


    QueryParams result = {0};
    if (!query || strlen(query) == 0) return result;

    int count = 1;
    for (const char* p = query; *p; p++) {
        if (*p == '&') count++;
    }

    result.params = malloc(sizeof(QueryParam) * count);
    result.count = 0;

    char* query_dup = strdup(query);
    char* token = strtok(query_dup, "&");

    while (token) {
        char* equal_sign = strchr(token, '=');
        if (equal_sign) {
            *equal_sign = '\0';
            char* key = token;
            char* value = equal_sign + 1;

            result.params[result.count].key = strdup(key);
            result.params[result.count].value = strdup(value);
            result.count++;
        }
        token = strtok(NULL, "&");
    }

    free(query_dup);
    return result;
}