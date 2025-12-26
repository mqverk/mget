#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "mget.h"

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    HttpResponse *mem = (HttpResponse *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) {
        log_error("Not enough memory for HTTP response");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

HttpResponse http_get(const char *url) {
    HttpResponse response = {.data = malloc(1), .size = 0};

    CURL *curl = curl_easy_init();
    if (!curl) {
        log_error("Failed to initialize CURL");
        return response;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "User-Agent: mget/1.0");
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        log_error("CURL error: %s", curl_easy_strerror(res));
        free(response.data);
        response.data = malloc(1);
        response.size = 0;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code != 200) {
        log_error("HTTP error: %ld", http_code);
        free(response.data);
        response.data = malloc(1);
        response.size = 0;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

void free_response(HttpResponse *resp) {
    if (resp && resp->data) {
        free(resp->data);
        resp->data = NULL;
        resp->size = 0;
    }
}
