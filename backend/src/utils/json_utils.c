#include "json_utils.h"
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <stdlib.h>

int parse_json(const char *json_str, KeyValuePair *pairs, int max_pairs)
{
    // printf("json_str: %s\n", json_str);
    cJSON *json = cJSON_Parse(json_str);
    if (json == NULL)
    {
        fprintf(stderr, "Error parsing JSON\n");
        return 0;
    }

    int pair_count = 0;
    cJSON *item = NULL;
    cJSON_ArrayForEach(item, json)
    {
        if (pair_count >= max_pairs)
        {
            break;
        }

        if (cJSON_IsString(item) && (item->valuestring != NULL))
        {
            strncpy(pairs[pair_count].key, item->string, sizeof(pairs[pair_count].key) - 1);
            strncpy(pairs[pair_count].value, item->valuestring, sizeof(pairs[pair_count].value) - 1);
            pair_count++;
        }
        else if (cJSON_IsNumber(item))
        {
            strncpy(pairs[pair_count].key, item->string, sizeof(pairs[pair_count].key) - 1);
            snprintf(pairs[pair_count].value, sizeof(pairs[pair_count].value), "%lf", item->valuedouble);
            pair_count++;
        }
        else if (cJSON_IsArray(item))
        {
            strncpy(pairs[pair_count].key, item->string, sizeof(pairs[pair_count].key) - 1);
            char *array_str = cJSON_Print(item);
            strncpy(pairs[pair_count].value, array_str, sizeof(pairs[pair_count].value) - 1);
            free(array_str);
            pair_count++;
        }
        else if (cJSON_IsObject(item))
        {
            strncpy(pairs[pair_count].key, item->string, sizeof(pairs[pair_count].key) - 1);
            char *object_str = cJSON_Print(item);
            strncpy(pairs[pair_count].value, object_str, sizeof(pairs[pair_count].value) - 1);
            free(object_str);
            pair_count++;
        }
    }

    cJSON_Delete(json);
    return pair_count;
}

char *convert_to_json(KeyValuePair *pairs, int pair_count)
{
    cJSON *json = cJSON_CreateObject();
    for (int i = 0; i < pair_count; i++)
    {
        cJSON_AddStringToObject(json, pairs[i].key, pairs[i].value);
    }

    char *json_string = cJSON_Print(json);
    cJSON_Delete(json);

    return json_string;
}

void add_key_value_pair(KeyValuePair *pairs, int *pair_count, const char *key, const char *value)
{
    strncpy(pairs[*pair_count].key, key, sizeof(pairs[*pair_count].key) - 1);
    strncpy(pairs[*pair_count].value, value, sizeof(pairs[*pair_count].value) - 1);
    (*pair_count)++;
}