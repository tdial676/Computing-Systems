#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "directory_tree.h"
#include "fat16.h"

const size_t MASTER_BOOT_RECORD_SIZE = 0x20B;

void follow(FILE *disk, directory_node_t *node, bios_parameter_block_t bpb) {
    directory_entry_t curr_entry;
    size_t read = fread(&curr_entry, sizeof(directory_entry_t), 1, disk);
    assert(read != 0);
    char *name = get_file_name(curr_entry);
    assert(name != NULL);
    while (strcmp(name, "\0") != 0) {
        if (is_hidden(curr_entry)) {
            size_t red = fread(&curr_entry, sizeof(directory_entry_t), 1, disk);
            assert(red != 0);
            free(name);
            name = get_file_name(curr_entry);
            continue;
        }
        long old = ftell(disk);
        size_t offset = get_offset_from_cluster(curr_entry.first_cluster, bpb);
        int seek1 = fseek(disk, offset, SEEK_SET);
        assert(seek1 == 0);
        if (!is_directory(curr_entry)) {
            char *data = malloc(curr_entry.file_size * sizeof(char));
            assert(data != NULL);
            size_t red = fread(data, curr_entry.file_size, 1, disk);
            assert(red != 0);
            file_node_t *child =
                init_file_node(name, curr_entry.file_size, (uint8_t *) data);
            add_child_directory_tree(node, (node_t *) child);
        }
        else {
            directory_node_t *new_directory = init_directory_node(name);
            add_child_directory_tree(node, (node_t *) new_directory);
            follow(disk, new_directory, bpb);
        }
        int seek2 = fseek(disk, old, SEEK_SET);
        assert(seek2 == 0);
        size_t red = fread(&curr_entry, sizeof(directory_entry_t), 1, disk);
        assert(red != 0);
        name = get_file_name(curr_entry);
    }
    free(name);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s <image filename>\n", argv[0]);
        return 1;
    }

    FILE *disk = fopen(argv[1], "r");
    if (disk == NULL) {
        fprintf(stderr, "No such image file: %s\n", argv[1]);
        return 1;
    }

    bios_parameter_block_t bpb;
    // Bypasses and reads disk to bpb
    int seek = fseek(disk, MASTER_BOOT_RECORD_SIZE, SEEK_CUR);
    assert(seek == 0);
    size_t read = fread(&bpb, sizeof(bpb), 1, disk);
    assert(read != 0);
    size_t location = get_root_directory_location(bpb);
    int seek2 = fseek(disk, location, SEEK_SET);
    assert(seek2 == 0);

    directory_node_t *root = init_directory_node(NULL);

    follow(disk, root, bpb);
    print_directory_tree((node_t *) root);
    create_directory_tree((node_t *) root);
    free_directory_tree((node_t *) root);

    int result = fclose(disk);
    assert(result == 0);
}
