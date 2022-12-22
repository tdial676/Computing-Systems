#include "directory_tree.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

const mode_t MODE = 0777;
const size_t SPACES = 4;

void init_node(node_t *node, char *name, node_type_t type) {
    if (name == NULL) {
        name = strdup("ROOT");
        assert(name != NULL);
    }
    node->name = name;
    node->type = type;
}

file_node_t *init_file_node(char *name, size_t size, uint8_t *contents) {
    file_node_t *node = malloc(sizeof(file_node_t));
    assert(node != NULL);
    init_node((node_t *) node, name, FILE_TYPE);
    node->size = size;
    node->contents = contents;
    return node;
}

directory_node_t *init_directory_node(char *name) {
    directory_node_t *node = malloc(sizeof(directory_node_t));
    assert(node != NULL);
    init_node((node_t *) node, name, DIRECTORY_TYPE);
    node->num_children = 0;
    node->children = NULL;
    return node;
}

void add_child_directory_tree(directory_node_t *dnode, node_t *child) {
    dnode->num_children++;
    dnode->children = realloc(dnode->children, dnode->num_children * sizeof(node_t *));
    assert(dnode->children != NULL);
    dnode->children[dnode->num_children - 1] = child;
    for (size_t i = 0; i < dnode->num_children; i++) {
        for (size_t j = i + 1; j < dnode->num_children; j++) {
            if (strcmp(dnode->children[i]->name, dnode->children[j]->name) > 0) {
                node_t *curr_node = dnode->children[i];
                dnode->children[i] = dnode->children[j];
                dnode->children[j] = curr_node;
            }
        }
    }
}
/* Helper to add the spaces then the name*/
void pretty_print(char *name, size_t indent) {
    for (size_t i = 0; i < indent; i++) {
        printf("%c", ' ');
    }
    printf("%s\n", name);
}

/*
Helper Method that takes in a node and level and prints ththat node name with
indent many spaces.
*/
void print_helper(node_t *node, size_t indent) {
    if (node->type == FILE_TYPE) {
        pretty_print(node->name, indent * SPACES);
    }
    else {
        directory_node_t *dnode = (directory_node_t *) node;
        pretty_print(dnode->base.name, indent * SPACES);
        for (size_t i = 0; i < dnode->num_children; i++) {
            print_helper(dnode->children[i], indent + 1);
        }
    }
}

void print_directory_tree(node_t *node) {
    print_helper(node, 0);
}

/*
Helper that takes in the current file path and adds a / to it.
*/
char *make_path(char *path) {
    char *new_path = malloc((strlen(path) + 2) * sizeof(char));
    strcpy(new_path, path);
    new_path[strlen(path)] = '/';
    new_path[strlen(path) + 1] = '\0';
    return new_path;
}
/*
Helper that takes in node and currentrly directory and
opens the file in that directory of add the directory
to that file.
*/
void create_directory_helper(node_t *node, char *directory) {
    char *new_path = make_path(directory);
    size_t size = strlen(new_path) + strlen(node->name) + 1;
    new_path = realloc(new_path, size * sizeof(char));
    strcat(new_path, node->name);
    if (node->type == FILE_TYPE) {
        file_node_t *fnode = (file_node_t *) node;
        FILE *file = fopen(new_path, "w");
        assert(file != NULL);
        size_t wrote = fwrite(fnode->contents, sizeof(u_int8_t), fnode->size, file);
        assert(wrote != 0);
        int closed = fclose(file);
        assert(closed == 0);
    }
    else {
        directory_node_t *dnode = (directory_node_t *) node;
        int dir = mkdir(new_path, MODE);
        assert(dir == 0);
        for (size_t i = 0; i < dnode->num_children; i++) {
            create_directory_helper(dnode->children[i], new_path);
        }
    }
    free(new_path);
}

void create_directory_tree(node_t *node) {
    create_directory_helper(node, ".");
}

void free_directory_tree(node_t *node) {
    if (node->type == FILE_TYPE) {
        file_node_t *fnode = (file_node_t *) node;
        free(fnode->contents);
    }
    else {
        assert(node->type == DIRECTORY_TYPE);
        directory_node_t *dnode = (directory_node_t *) node;
        for (size_t i = 0; i < dnode->num_children; i++) {
            free_directory_tree(dnode->children[i]);
        }
        free(dnode->children);
    }
    free(node->name);
    free(node);
}
