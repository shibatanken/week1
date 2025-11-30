#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <cjson/cJSON.h>

typedef struct
{
    char key[256];
    char value[256];
} KeyValuePair;

int parse_json(const char *json_str, KeyValuePair *pairs, int max_pairs);
char *convert_to_json(KeyValuePair *pairs, int pair_count);
void add_key_value_pair(KeyValuePair *pairs, int *pair_count, const char *key, const char *value);

#endif // JSON_UTILS_H