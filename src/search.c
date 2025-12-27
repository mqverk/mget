#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "mget.h"

void search_plugins(const char *query) {
    char url[MAX_URL_LEN];
    // Search for both plugins and mods: [["project_type:plugin"],["project_type:mod"]]
    // URL encoded: %5B%5B%22project_type:plugin%22%5D,%5B%22project_type:mod%22%5D%5D
    snprintf(url, MAX_URL_LEN, "%s/search?query=%s&limit=10", API_BASE, query);

    log_info("Searching Modrinth for '%s'...", query);

    HttpResponse response = http_get(url);
    if (response.size == 0) {
        log_error("Failed to search for plugins");
        free_response(&response);
        return;
    }

    json_object *root = json_tokener_parse(response.data);
    if (!root) {
        log_error("Failed to parse JSON response");
        free_response(&response);
        return;
    }

    json_object *hits;
    if (!json_object_object_get_ex(root, "hits", &hits)) {
        log_error("No hits found in response");
        json_object_put(root);
        free_response(&response);
        return;
    }

    int hits_count = json_object_array_length(hits);
    if (hits_count == 0) {
        log_info("No plugins found matching '%s'", query);
        json_object_put(root);
        free_response(&response);
        return;
    }

    printf("\n ID (Slug)                 | Name                           | Author\n");
    printf("--------------------------------------------------------------------------------\n");

    for (int i = 0; i < hits_count; i++) {
        json_object *hit = json_object_array_get_idx(hits, i);

        json_object *slug_obj, *title_obj, *author_obj;
        const char *slug = NULL;
        const char *title = NULL;
        const char *author = NULL;

        if (json_object_object_get_ex(hit, "slug", &slug_obj)) {
            slug = json_object_get_string(slug_obj);
        }
        if (json_object_object_get_ex(hit, "title", &title_obj)) {
            title = json_object_get_string(title_obj);
        }
        if (json_object_object_get_ex(hit, "author", &author_obj)) {
            author = json_object_get_string(author_obj);
        }

        printf(" %-25s | %-30s | %s\n", slug ? slug : "N/A", title ? title : "N/A", author ? author : "N/A");
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("\n");
    log_info("Found %d plugins matching '%s'.", hits_count, query);

    json_object_put(root);
    free_response(&response);
}
