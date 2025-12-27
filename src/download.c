#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "mget.h"

#define DOWNLOADS_DIR "downloads"

// Create downloads directory if it doesn't exist
static int ensure_downloads_dir(void) {
    struct stat st = {0};
    if (stat(DOWNLOADS_DIR, &st) == -1) {
        if (mkdir(DOWNLOADS_DIR, 0755) == -1) {
            log_error("Failed to create downloads directory");
            return 0;
        }
    }
    return 1;
}

// Build full path for downloaded file
static void get_file_path(const char *filename, char *filepath, size_t filepath_size) {
    snprintf(filepath, filepath_size, "%s/%s", DOWNLOADS_DIR, filename);
}

void download_plugin(const char *slug, const char *version, const char *server_type) {
    char url[MAX_URL_LEN];
    
    // Ensure downloads directory exists
    if (!ensure_downloads_dir()) {
        return;
    }
    
    log_info("Resolving version for '%s' (v%s%s%s)...", 
        slug, version, 
        server_type ? ", " : "", 
        server_type ? server_type : "");

    // Build the versions URL with filters
    if (server_type) {
        snprintf(url, MAX_URL_LEN, "%s/project/%s/version?game_versions=%%5B%%22%s%%22%%5D&loaders=%%5B%%22%s%%22%%5D", 
            API_BASE, slug, version, server_type);
    } else {
        snprintf(url, MAX_URL_LEN, "%s/project/%s/version?game_versions=%%5B%%22%s%%22%%5D", 
            API_BASE, slug, version);
    }

    HttpResponse versions_response = http_get(url);
    if (versions_response.size == 0) {
        log_error("Failed to fetch versions");
        free_response(&versions_response);
        return;
    }

    json_object *versions = json_tokener_parse(versions_response.data);
    if (!versions || !json_object_is_type(versions, json_type_array)) {
        log_error("Failed to parse versions JSON");
        if (versions) json_object_put(versions);
        free_response(&versions_response);
        return;
    }

    int versions_count = json_object_array_length(versions);
    if (versions_count == 0) {
        log_error("No versions found for '%s' with version '%s'%s%s", 
            slug, version,
            server_type ? " and loader '" : "",
            server_type ? server_type : "");
        json_object_put(versions);
        free_response(&versions_response);
        return;
    }

    // Get the first version (most recent)
    json_object *version_obj = json_object_array_get_idx(versions, 0);
    
    json_object *files_obj;
    if (!json_object_object_get_ex(version_obj, "files", &files_obj) || json_object_array_length(files_obj) == 0) {
        log_error("No files found in version");
        json_object_put(versions);
        free_response(&versions_response);
        return;
    }

    json_object *file = json_object_array_get_idx(files_obj, 0);
    json_object *url_obj, *filename_obj;
    const char *download_url = NULL;
    const char *filename = NULL;
    
    if (json_object_object_get_ex(file, "url", &url_obj)) {
        download_url = json_object_get_string(url_obj);
    }
    if (json_object_object_get_ex(file, "filename", &filename_obj)) {
        filename = json_object_get_string(filename_obj);
    }

    if (!download_url || !filename) {
        log_error("Could not extract download URL or filename");
        json_object_put(versions);
        free_response(&versions_response);
        return;
    }

    log_info("Downloading file: %s", filename);

    // Download the file
    HttpResponse file_response = http_get(download_url);
    if (file_response.size == 0) {
        log_error("Failed to download file");
        json_object_put(versions);
        free_response(&versions_response);
        free_response(&file_response);
        return;
    }

    // Build full file path
    char filepath[MAX_FILENAME + 32];
    get_file_path(filename, filepath, sizeof(filepath));

    // Save file
    FILE *fp = fopen(filepath, "wb");
    if (!fp) {
        log_error("Failed to open file for writing");
        json_object_put(versions);
        free_response(&versions_response);
        free_response(&file_response);
        return;
    }

    size_t written = fwrite(file_response.data, 1, file_response.size, fp);
    fclose(fp);

    if (written != file_response.size) {
        log_error("Failed to write complete file");
        json_object_put(versions);
        free_response(&versions_response);
        free_response(&file_response);
        return;
    }

    log_success("Saved as %s/%s", DOWNLOADS_DIR, filename);

    json_object_put(versions);
    free_response(&versions_response);
    free_response(&file_response);
}
