#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "mget.h"

void print_help(const char *program_name) {
    printf("mget - Modrinth Plugin & Mod Downloader\n\n");
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -f, --find <name>        Search for a project by name\n");
    printf("  -p, --plugin <slug>      Plugin slug to download\n");
    printf("  -m, --mod <slug>         Mod slug to download\n");
    printf("  -v, --version <version>  Target Minecraft version\n");
    printf("  -l, --loader <type>      Mod loader (forge, fabric, neoforge, etc.)\n");
    printf("  -s, --server <type>      Server type for plugins (paper, spigot, bukkit, etc.)\n");
    printf("  -h, --help               Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s -f luckperms          # Search for projects\n", program_name);
    printf("  %s -p luckperms -v 1.21.8 -s paper              # Download plugin\n", program_name);
    printf("  %s -m travelersbackpack -v 1.20.1 -l fabric     # Download mod\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_help(argv[0]);
        return 0;
    }

    Options options = {
        .query = NULL,
        .plugin_slug = NULL,
        .version = NULL,
        .server_type = NULL
    };

    static struct option long_options[] = {
        {"find", required_argument, 0, 'f'},
        {"plugin", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {"version", required_argument, 0, 'v'},
        {"loader", required_argument, 0, 'l'},
        {"server", required_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "f:p:m:v:l:s:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'f':
                options.query = optarg;
                break;
            case 'p':
                options.plugin_slug = optarg;
                break;
            case 'm':
                options.plugin_slug = optarg;  // Reuse the same field for mod slug
                break;
            case 'v':
                options.version = optarg;
                break;
            case 'l':
                options.server_type = optarg;  // Reuse for loader type
                break;
            case 's':
                options.server_type = optarg;
                break;
            case 'h':
                print_help(argv[0]);
                return 0;
            default:
                return 1;
        }
    }

    if (options.query) {
        search_plugins(options.query);
    } else if (options.plugin_slug) {
        if (!options.version) {
            log_error("Error: version is required with --plugin or --mod");
            return 1;
        }
        download_plugin(options.plugin_slug, options.version, options.server_type);
    } else {
        log_error("Error: either --find, --plugin, or --mod must be specified");
        print_help(argv[0]);
        return 1;
    }

    return 0;
}
