#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "directory_tree.h"

const unsigned MKDIR_MODE = 0777;

/**
 * Performs a binary search to find the entry with the given name in a directory.
 * This assumes that the directory's entries are sorted by name.
 * Returns NULL if no matching entry is found.
 */
node_t *get_child(directory_node_t *directory, char *name) {
    size_t left = 0;
    size_t right = directory->num_children;
    while (left < right) {
        size_t mid = (left + right) / 2;
        node_t *mid_child = directory->children[mid];
        int cmp = strcmp(name, mid_child->name);
        if (cmp == 0) {
            return mid_child;
        }

        if (cmp < 0) {
            right = mid;
        }
        else /* cmp > 0 */ {
            left = mid + 1;
        }
    }
    return NULL;
}

/**
 * Adds a file with the given path and contents to the directory tree.
 * Builds any missing intermediate directories.
 */
void add_file(directory_node_t *directory, char *path, char *contents) {
    char *remaining_path = path;
    while (true) {
        // Identify the next file/directory name in the path
        char *slash = strchr(remaining_path, '/');
        if (slash != NULL) {
            *slash = '\0';
        }

        node_t *child = get_child(directory, remaining_path);
        if (slash == NULL) {
            // This is the last part of the path, so it represents a file
            if (child != NULL) {
                fprintf(stderr, "File '%s' already exists\n", path);
                abort();
            }
            char *path_copy = strdup(remaining_path);
            assert(path_copy != NULL);
            size_t file_size = strlen(contents);
            void *contents_copy = malloc(sizeof(char[file_size]));
            assert(contents_copy != NULL);
            memcpy(contents_copy, contents, sizeof(char[file_size]));
            child = (node_t *) init_file_node(path_copy, file_size, contents_copy);
            add_child_directory_tree(directory, child);
            break;
        }

        // This is an intermediate directory
        if (child == NULL) {
            char *path_copy = strdup(remaining_path);
            assert(path_copy != NULL);
            child = (node_t *) init_directory_node(path_copy);
            add_child_directory_tree(directory, child);
        }
        else {
            // If the child already exists, it should be a directory
            assert(child->type == DIRECTORY_TYPE);
        }
        directory = (directory_node_t *) child;
        remaining_path = slash + 1;
    }
}

int main(int argc, char **argv) {
    // Program should be invoked as "bin/test_tree test-input-file.txt output-files"
    assert(argc == 3);
    FILE *test_input = fopen(argv[1], "r");
    assert(test_input != NULL);

    directory_node_t *root = init_directory_node(NULL);
    char *line = NULL;
    size_t line_capacity = 0;
    while (getline(&line, &line_capacity, test_input) > 0) {
        /* Separate each line of the input file into path:contents
         * and add the given file to the directory tree */
        char *colon = strchr(line, ':');
        assert(colon != NULL);
        *colon = '\0';
        add_file(root, line, colon + 1);
    }
    free(line);
    int result = fclose(test_input);
    assert(result == 0);

    print_directory_tree((node_t *) root);
    result = mkdir(argv[2], MKDIR_MODE);
    assert(result == 0);
    result = chdir(argv[2]);
    assert(result == 0);
    create_directory_tree((node_t *) root);
    free_directory_tree((node_t *) root);
}
