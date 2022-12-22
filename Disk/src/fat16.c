#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "fat16.h"

typedef enum {
    READ_ONLY = 0x1,
    HIDDEN = 0x2,
    SYSTEM = 0x4,
    VOLUME_LABEL = 0x8,
    SUBDIRECTORY = 0x10,
    ARCHIVE = 0x20,
    DEVICE = 0x40
} fat_attribute_t;

const char DELETED = 0xE5;

size_t get_root_directory_location(bios_parameter_block_t bpb) {
    size_t fat_sectors = (size_t) bpb.num_fats * bpb.sectors_per_fat;
    return (1 + bpb.reserved_sectors + fat_sectors) * bpb.bytes_per_sector;
}

size_t get_offset_from_cluster(size_t cluster, bios_parameter_block_t bpb) {
    return get_root_directory_location(bpb) +
           bpb.max_root_entries * sizeof(directory_entry_t) +
           (cluster - 2) * bpb.sectors_per_cluster * bpb.bytes_per_sector;
}

bool is_directory(directory_entry_t entry) {
    return (entry.attribute & SUBDIRECTORY) != 0;
}

bool is_hidden(directory_entry_t entry) {
    return entry.filename[0] == '.' || entry.filename[1] == '.' || entry.attribute == 0 ||
           (entry.attribute & (HIDDEN | SYSTEM | VOLUME_LABEL)) != 0;
}

char *get_file_name(directory_entry_t entry) {
    char *full = malloc(sizeof(entry.filename) + 1 + sizeof(entry.extension) + 1);
    assert(full != NULL);
    size_t full_index = 0;
    size_t filename_index = 0;
    while (filename_index < sizeof(entry.filename)) {
        char filename_char = entry.filename[filename_index++];
        if (filename_char == ' ') {
            break;
        }

        full[full_index++] = filename_char == DELETED ? '?' : filename_char;
    }
    if (entry.extension[0] != ' ') {
        full[full_index++] = '.';
        size_t extension_index = 0;
        while (extension_index < sizeof(entry.extension)) {
            char extension_char = entry.extension[extension_index++];
            if (extension_char == ' ') {
                break;
            }

            full[full_index++] = extension_char;
        }
    }
    full[full_index] = '\0';
    return full;
}
