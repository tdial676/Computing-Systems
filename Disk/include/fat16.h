#ifndef FAT16_H
#define FAT16_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Represents the layout of a FAT16 BIOS Parameter Block on disk.
 * `__attribute__((__packed__))` prevents C from adding padding bytes between fields.
 */
typedef struct __attribute__((__packed__)) {
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t max_root_entries;
    uint16_t logical_sectors;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat;
    uint8_t padding[19];
    char volume_name[11];
    char type[8];
} bios_parameter_block_t;

/**
 * Represents the layout of a FAT16 Directory Entry on disk
 * `__attribute__((__packed__))` prevents C from adding padding bytes between fields.
 */
typedef struct __attribute__((__packed__)) {
    char filename[8];
    char extension[3];
    uint8_t attribute;
    char reserved[8];
    uint16_t high_size;
    char time[2];
    char date[2];
    uint16_t first_cluster;
    uint32_t file_size;
} directory_entry_t;

/**
 * Computes the position of the start of the root directory on a FAT16 disk.
 *
 * @param bpb the BIOS Parameter Block of the FAT16 disk
 * @return the index of the byte on the disk where the root directory begins
 */
size_t get_root_directory_location(bios_parameter_block_t bpb);

/**
 * Converts a cluster number to a position on a FAT16 disk.
 *
 * @param cluster the cluster number to look up
 * @param bpb the BIOS Parameter Block of the FAT16 disk
 * @return the index of the byte on the disk where the cluster begins
 */
size_t get_offset_from_cluster(size_t cluster, bios_parameter_block_t bpb);

/**
 * Computes whether a directory entry represents a subdirectory.
 *
 * @param entry the directory entry
 * @return `true` if the entry represents a subdirectory, `false` if it is a file
 */
bool is_directory(directory_entry_t entry);

/**
 * Computes whether a directory entry represents a hidden entry.
 * Hidden entries should be ignored when building the directory tree.
 *
 * @param entry the directory entry
 * @return `true` iff the entry is hidden
 */
bool is_hidden(directory_entry_t entry);

/**
 * Compute's a directory entry's filename.
 * If the entry is marked deleted, this will approximate its previous filename.
 *
 * @param entry the directory entry
 * @return a heap-allocated string containing the entry's filename
 */
char *get_file_name(directory_entry_t entry);

#endif /* FAT16_H */
