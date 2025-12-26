#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "mget.h"

void download_plugin(const char *slug, const char *version, const char *server_type) {
    char url[MAX_URL_LEN];
    
    log_info("Resolving version for '%s' (v%s%s%s)...", 
        slug, version, 
        server_type ? ", " : "", 
        server_type ? server_type : "");

    // Get project info
    snprintf(url, MAX_URL_LEN, "%s/project/%s", API_BASE, slug);
    HttpResponse project_response = http_get(url);
    if (project_response.size == 0) {
        log_error("Failed to fetch project info");
        free_response(&project_response);
        return;
    }

    json_object *project = json_tokener_parse(project_response.data);
    if (!project) {
        log_error("Failed to parse project JSON");
        free_response(&project_response);
        return;
    }

    // Get versions
    snprintf(url, MAX_URL_LEN, "%s/project/%s/versions", API_BASE, slug);
    HttpResponse versions_response = http_get(url);
    if (versions_response.size == 0) {
        log_error("Failed to fetch versions");
        json_object_put(project);
        free_response(&project_response);
        free_response(&versions_response);
        return;
    }

    json_object *versions = json_tokener_parse(versions_response.data);
    if (!versions || !json_object_is_type(versions, json_type_array)) {
        log_error("Failed to parse versions JSON");
        json_object_put(project);
        free_response(&project_response);
        free_response(&versions_response);
        return;
    }

    // Find matching version
    const char *download_url = NULL;
    const char *filename = NULL;
    int versions_count = json_object_array_length(versions);

    for (int i = 0; i < versions_count; i++) {
        json_object *v = json_object_array_get_idx(versions, i);
        
        json_object *game_versions_obj;
        if (!json_object_object_get_ex(v, "game_versions", &game_versions_obj)) {
            continue;
        }

        int found_version = 0;
        int gv_count = json_object_array_length(game_versions_obj);
        for (int j = 0; j < gv_count; j++) {
            json_object *gv = json_object_array_get_idx(game_versions_obj, j);
            const char *gv_str = json_object_get_string(gv);
            if (gv_str && strcmp(gv_str, version) == 0) {
                found_version = 1;
                break;
            }
        }

        if (!found_version) continue;

        // Check loaders if server_type specified
        if (server_type) {
            json_object *loaders_obj;
            if (!json_object_object_get_ex(v, "loaders", &loaders_obj)) {
                continue;
            }

            int found_loader = 0;
            int loaders_count = json_object_array_length(loaders_obj);
            for (int j = 0; j < loaders_count; j++) {
                json_object *loader = json_object_array_get_idx(loaders_obj, j);
                const char *loader_str = json_object_get_string(loader);
                if (loader_str && strcmp(loader_str, server_type) == 0) {
                    found_loader = 1;
                    break;
                }
            }

            if (!found_loader) continue;
        }

        // Get file info
        json_object *files_obj;
        if (!json_object_object_get_ex(v, "files", &files_obj)) {
            continue;
        }

        if (json_object_array_length(files_obj) > 0) {
            json_object *file = json_object_array_get_idx(files_obj, 0);
            json_object *url_obj, *filename_obj;
            
            if (json_object_object_get_ex(file, "url", &url_obj)) {
                download_url = json_object_get_string(url_obj);
            }
            if (json_object_object_get_ex(file, "filename", &filename_obj)) {
                filename = json_object_get_string(filename_obj);
            }
            break;
        }
    }

    if (!download_url || !filename) {
        log_error("Could not find a matching version for '%s' with version '%s'%s%s", 
            slug, version,
            server_type ? " and server type '" : "",
            server_type ? server_type : "");
        json_object_put(project);
        json_object_put(versions);
        free_response(&project_response);
        free_response(&versions_response);
        return;
    }

    log_info("Downloading file: %s", filename);

    // Download the file
    HttpResponse file_response = http_get(download_url);
    if (file_response.size == 0) {
        log_error("Failed to download file");
        json_object_put(project);
        json_object_put(versions);
        free_response(&project_response);
        free_response(&versions_response);
        free_response(&file_response);
        return;
    }

    // Save file
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        log_error("Failed to open file for writing");
        json_object_put(project);
        json_object_put(versions);
        free_response(&project_response);
        free_response(&versions_response);
        free_response(&file_response);
        return;
    }

    size_t written = fwrite(file_response.data, 1, file_response.size, fp);
    fclose(fp);

    if (written != file_response.size) {
        log_error("Failed to write complete file");
        json_object_put(project);
        json_object_put(versions);
        free_response(&project_response);
        free_response(&versions_response);
        free_response(&file_response);
        return;
    }

    log_success("Saved as %s", filename);

    json_object_put(project);
    json_object_put(versions);
    free_response(&project_response);
    free_response(&versions_response);
    free_response(&file_response);
}
