#ifndef ASSET_EXTRACTOR_H
#define ASSET_EXTRACTOR_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    ROM_SUCCESS,
    ROM_CANNOT_OPEN,
    ROM_INVALID_FILESIZE,
    ROM_CHECKSUM_FAILED,
} ROMError;

bool assetextract_check_needs_extract();
ROMError assetextract_read_rom(const char* filename);

void assetextract_run();
float assetextract_progress(const char** curr_file);

void* assetextract_get_asset(const char* name, size_t* size);

#endif