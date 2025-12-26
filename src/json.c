#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "mget.h"

char *json_get_string(const char *json, const char *key) {
    if (!json || !key) return NULL;

    json_object *root = json_tokener_parse(json);
    if (!root) return NULL;

    json_object *obj;
    if (!json_object_object_get_ex(root, key, &obj)) {
        json_object_put(root);
        return NULL;
    }

    const char *value = json_object_get_string(obj);
    char *result = NULL;
    
    if (value) {
        result = malloc(strlen(value) + 1);
        strcpy(result, value);
    }

    json_object_put(root);
    return result;
}

char *json_get_project_by_slug(const char *json, const char *slug) {
    if (!json || !slug) return NULL;

    json_object *root = json_tokener_parse(json);
    if (!root || !json_object_is_type(root, json_type_array)) {
        if (root) json_object_put(root);
        return NULL;
    }

    int count = json_object_array_length(root);
    for (int i = 0; i < count; i++) {
        json_object *obj = json_object_array_get_idx(root, i);
        json_object *slug_obj;
        
        if (json_object_object_get_ex(obj, "slug", &slug_obj)) {
            const char *obj_slug = json_object_get_string(slug_obj);
            if (obj_slug && strcmp(obj_slug, slug) == 0) {
                const char *json_str = json_object_to_json_string(obj);
                char *result = malloc(strlen(json_str) + 1);
                strcpy(result, json_str);
                json_object_put(root);
                return result;
            }
        }
    }

    json_object_put(root);
    return NULL;
}
