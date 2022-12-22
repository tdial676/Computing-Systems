#ifndef READ_CLASS_H
#define READ_CLASS_H

#include <stdio.h>
#include "class_file.h"

/**
 * Finds the method with the given name and signature.
 * The descriptor is necessary because Java allows method overloading.
 * This only needs to be called directly to invoke main();
 * for the invokestatic instruction, use find_method_from_index().
 *
 * @param name the method name, e.g. "factorial"
 * @param descriptor the method descriptor string, e.g. "(I)I"
 * @param class the parsed class file
 * @return the method if it was found, NULL otherwise
 */
method_t *find_method(const char *name, const char *descriptor,
                      const class_file_t *class);

/**
 * Finds the method corresponding to the given constant pool index.
 *
 * @param index the constant pool index of the Methodref to call
 * @param class the parsed class file
 * @return the method if it was found, NULL otherwise
 */
method_t *find_method_from_index(uint16_t index, const class_file_t *class);

/**
 * Gets the number of (integer) parameters a method takes.
 * Uses the descriptor string of the method to determine its signature.
 */
uint16_t get_number_of_parameters(const method_t *method);

/**
 * Reads an entire class file.
 * The end of the parsed methods array is marked by a method with a NULL name.
 *
 * @param class_file the open file to read
 * @return the parsed class file, allocated on the heap
 */
class_file_t *get_class(FILE *class_file);

/**
 * Frees the memory used by a parsed class file.
 *
 * @param class the parsed class file
 */
void free_class(class_file_t *class);

#endif /* READ_CLASS_H */
