#ifndef MGET_H
#define MGET_H

#include <stddef.h>

#define API_BASE "https://api.modrinth.com/v2"
#define MAX_URL_LEN 512
#define MAX_FILENAME 256
#define MAX_JSON_SIZE 1024 * 1024 * 10

typedef struct {
    char *query;
    char *plugin_slug;
    char *version;
    char *server_type;
} Options;

// network.c
typedef struct {
    char *data;
    size_t size;
} HttpResponse;

HttpResponse http_get(const char *url);
void free_response(HttpResponse *resp);

// search.c
void search_plugins(const char *query);

// download.c
void download_plugin(const char *slug, const char *version, const char *server_type);

// json.c
char *json_get_string(const char *json, const char *key);
char *json_get_project_by_slug(const char *json, const char *slug);

// logs.c
void log_info(const char *format, ...);
void log_error(const char *format, ...);
void log_success(const char *format, ...);

#endif
