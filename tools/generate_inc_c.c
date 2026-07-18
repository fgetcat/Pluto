#define PUSH_ASSET
#define SEPARATOR ,
#define ASSETS Asset assets[] =
#define STRUCT_ONLY

#include "src/saturn/asset_extractor/assets.c"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define LENGTH(x) (sizeof(x) - 1)

char* parentdir(char* path) {
    for (int i = strlen(path) - 1; i >= 0; i-- ) {
        if (path[i] == '/') {
            path[i + 1] = 0;
            return path;
        }
    }
    return path;
}

void mkdir_if_not_exist(char* path) {
    struct stat st;
    if (stat(path, &st) == -1)
        mkdir(path, 0744);
}

void mkdirs(char* path) {
    int len = strlen(path);
    for (int i = 1; i < len; i++) {
        if (path[i] != '/') continue;

        path[i] = 0;
        mkdir_if_not_exist(path);
        path[i] = '/';
    }
}

void generate_file(const char* name) {
    if (access(name, F_OK) == 0) return;
    
    int len = strlen(name);
    char dirname[len + 1];
    strcpy(dirname, name);
    mkdirs(parentdir(dirname));

    FILE* f = fopen(name, "w");
    for (uint64_t i = LENGTH("build/us_pc/"); i < len - LENGTH(".inc.c"); i++) {
        fprintf(f, "0x%02x,", name[i]);
    }
    fprintf(f, "0x00\n");
    fclose(f);
}

int main(int argc, char** argv) {
    if (argc >= 2) {
        generate_file(argv[1]);
        return 0;
    }
    
    for (uint64_t i = 0; i < sizeof(assets) / sizeof(*assets); i++) {
        Asset* asset = &assets[i];

        if (
            strncmp(asset->name, "actors", 6) != 0 &&
            strncmp(asset->name, "levels", 6) != 0 &&
            strncmp(asset->name, "textures", 8) != 0
        ) continue;

        int dest_size = LENGTH("build/us_pc/.inc.c") + strlen(asset->name) + 1;
        char filename[dest_size];
        sprintf(filename, "build/us_pc/%s.inc.c", asset->name);
        generate_file(filename);
    }

    return 0;
}
