#ifndef DIRECTORY_TREE_H
#define DIRECTORY_TREE_H

#include <inttypes.h>
#include <stddef.h>

/** The possible types of a `node_t` (either a file or a directory) */
typedef enum { FILE_TYPE, DIRECTORY_TYPE } node_type_t;

/** A node in a directory tree */
typedef struct {
    /**
     * The type of node (either a file or a directory).
     * If `type == FILE_TYPE`, the `node_t *` can be cast to a `file_node_t *`.
     * If `type == DIRECTORY_TYPE`, the `node_t *` can be cast to a `directory_node_t *`.
     */
    node_type_t type;
    /** The name of the file or directory */
    char *name;
} node_t;

/** A node representing a file in a directory tree */
typedef struct {
    /** The node's type (`FILE_TYPE`) and filename */
    node_t base;
    /** The size of the file's contents in bytes */
    size_t size;
    /** The file's contents. This points to a byte array of length `size`. */
    uint8_t *contents;
} file_node_t;

/** A node representing a directory in a directory tree */
typedef struct {
    /** The node's type (`DIRECTORY_TYPE`) and filename */
    node_t base;
    /** The number of children (files and subdirectories) in this directory */
    size_t num_children;
    /**
     * The directory's children. This points to an array
     * of `node_t *`s of length `num_children`.
     * The children are sorted by their filenames (duplicates are not permitted).
     * This can be `NULL` if `num_children == 0`.
     */
    node_t **children;
} directory_node_t;

/**
 * Creates a new file node with the given filename and contents.
 *
 * @param name the filename of the file.
 *   This string must be heap-allocated, and the function takes ownership of it.
 * @param size the number of bytes in the file
 * @param contents the bytes that make up the contents of the file.
 *   This must be a heap-allocated array of length `size`,
 *   and the function takes ownership of it.
 * @return a heap-allocated pointer to the file node
 */
file_node_t *init_file_node(char *name, size_t size, uint8_t *contents);

/**
 * Creates a new directory node with the given name.
 * The directory initially has no children.
 *
 * @param name the filename of the directory.
 *   This string must be heap-allocated, and the function takes ownership of it.
 *   `name` can be `NULL` for the root directory, and will be replaced by "ROOT".
 * @return a heap-allocated pointer to the directory node
 */
directory_node_t *init_directory_node(char *name);

/**
 * Adds a child node to a directory.
 * The directory's children must remain in sorted order.
 *
 * @param dnode the parent directory to add `child` to
 * @param child the child file or directory to add to `dnode`.
 *   This node must be heap-allocated, and the function takes ownership of it.
 */
void add_child_directory_tree(directory_node_t *dnode, node_t *child);

/**
 * Prints all the subdirectories and files in a directory tree.
 * See the project spec for the formatting requirements of this printed tree.
 *
 * @param node the root of the directory tree. You may assume this is a directory node.
 */
void print_directory_tree(node_t *node);

/**
 * Creates all files and directories in a directory tree.
 * The files and directories are created relative to the current working directory.
 *
 * @param node the root of the directory tree. You may assume this is a directory node.
 */
void create_directory_tree(node_t *node);

/**
 * Recursively frees all nodes in a directory tree.
 *
 * @param node a file or directory node to free. This may be a file or directory.
 *   The node and all its descendants (if it's a directory) must be heap-allocated.
 */
void free_directory_tree(node_t *node);

#endif /* DIRECTORY_TREE_H */
