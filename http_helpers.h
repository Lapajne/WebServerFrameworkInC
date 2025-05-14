#ifndef HTTP_HELPERS_H
#define HTTP_HELPERS_H

#define MAX_PARAMS 255

char* create_response(const char* html);
char* get_template(char* template_name);

typedef struct {
    char* key;
    char* value;
} Param;

Param* parse_params(const char* data, int* param_count_out);

#endif // HTTP_HELPERS_H
