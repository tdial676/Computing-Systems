#include "read_class.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const u4 CLASS_MAGIC = 0xCAFEBABE;
const u2 IS_STATIC = 0x0008;

/*
 * Functions for reading unsigned big-endian integers. We can't read directly
 * into a u2 or u4 variable because x86 stores integers in little-endian.
 */
u1 read_u1(FILE *class_file) {
    int result = fgetc(class_file);
    assert(result != EOF && "Reached end of file prematurely");
    return result;
}
u2 read_u2(FILE *class_file) {
    return (u2) read_u1(class_file) << 8 | read_u1(class_file);
}
u4 read_u4(FILE *class_file) {
    return (u4) read_u2(class_file) << 16 | read_u2(class_file);
}

u2 constant_pool_size(cp_info *constant_pool) {
    cp_info *constant = constant_pool;
    while (constant->info != NULL) {
        constant++;
    }
    return constant - constant_pool;
}

cp_info *get_constant(cp_info *constant_pool, u2 index) {
    assert(0 < index && index <= constant_pool_size(constant_pool) &&
           "Invalid constant pool index");
    // Convert 1-indexed index to 0-indexed index
    return &constant_pool[index - 1];
}

CONSTANT_NameAndType_info *get_method_name_and_type(cp_info *constant_pool, u2 index) {
    cp_info *method_constant = get_constant(constant_pool, index);
    assert(method_constant->tag == CONSTANT_Methodref && "Expected a MethodRef");
    CONSTANT_FieldOrMethodref_info *method_ref = method_constant->info;
    cp_info *name_and_type_constant =
        get_constant(constant_pool, method_ref->name_and_type_index);
    assert(name_and_type_constant->tag == CONSTANT_NameAndType &&
           "Expected a NameAndType");
    return name_and_type_constant->info;
}

u2 get_number_of_parameters(const method_t *method) {
    // Type descriptors will always have the length ( + #params + ) + return type
    char *end = strchr(method->descriptor, ')');
    char *start = strchr(method->descriptor, '(');

    u2 params = 0;

    for (start++; start < end; start++) {
        if (start[0] == '[') {
            start++;
        }
        params++;
    }

    return params;
}

method_t *find_method(const char *name, const char *descriptor,
                      const class_file_t *class) {
    for (method_t *method = class->methods; method->name != NULL; method++) {
        if (strcmp(method->name, name) == 0 &&
            strcmp(method->descriptor, descriptor) == 0) {
            return method;
        }
    }
    return NULL;
}

method_t *find_method_from_index(u2 index, const class_file_t *class) {
    CONSTANT_NameAndType_info *name_and_type =
        get_method_name_and_type(class->constant_pool, index);
    cp_info *name = get_constant(class->constant_pool, name_and_type->name_index);
    assert(name->tag == CONSTANT_Utf8 && "Expected a UTF8");
    cp_info *descriptor =
        get_constant(class->constant_pool, name_and_type->descriptor_index);
    assert(descriptor->tag == CONSTANT_Utf8 && "Expected a UTF8");
    return find_method(name->info, descriptor->info, class);
}

class_header_t get_class_header(FILE *class_file) {
    class_header_t header;
    header.magic = read_u4(class_file);
    assert(header.magic == CLASS_MAGIC);
    header.major_version = read_u2(class_file);
    header.minor_version = read_u2(class_file);
    return header;
}

cp_info *get_constant_pool(FILE *class_file) {
    // Constant pool count includes unused constant at index 0
    u2 constant_pool_count = read_u2(class_file) - 1;
    cp_info *constant_pool = malloc(sizeof(cp_info[constant_pool_count + 1]));
    assert(constant_pool != NULL && "Failed to allocate constant pool");

    cp_info *constant = constant_pool;
    while (constant_pool_count > 0) {
        constant->tag = read_u1(class_file);
        switch (constant->tag) {
            case CONSTANT_Utf8: {
                u2 length = read_u2(class_file);
                char *info = malloc(length + 1);
                assert(info != NULL && "Failed to allocate UTF8 constant");
                size_t bytes_read = fread(info, 1, length, class_file);
                assert(bytes_read == length && "Failed to read UTF8 constant");
                info[length] = '\0';
                constant->info = info;
                break;
            }

            case CONSTANT_Integer: {
                CONSTANT_Integer_info *value = malloc(sizeof(*value));
                assert(value != NULL && "Failed to allocate integer constant");
                value->bytes = read_u4(class_file);
                constant->info = value;
                break;
            }

            case CONSTANT_Class: {
                CONSTANT_Class_info *value = malloc(sizeof(*value));
                assert(value != NULL && "Failed to allocate class constant");
                value->string_index = read_u2(class_file);
                constant->info = value;
                break;
            }

            case CONSTANT_Methodref:
            case CONSTANT_Fieldref: {
                CONSTANT_FieldOrMethodref_info *value = malloc(sizeof(*value));
                assert(value != NULL && "Failed to allocate FieldRef/MethodRef constant");
                value->class_index = read_u2(class_file);
                value->name_and_type_index = read_u2(class_file);
                constant->info = value;
                break;
            }

            case CONSTANT_NameAndType: {
                CONSTANT_NameAndType_info *value = malloc(sizeof(*value));
                assert(value != NULL && "Failed to allocate NameAndType constant");
                value->name_index = read_u2(class_file);
                value->descriptor_index = read_u2(class_file);
                constant->info = value;
                break;
            }

            default:
                fprintf(stderr, "Unknown constant type %d\n", constant->tag);
                assert(false);
        }
        constant++;
        constant_pool_count--;
    }

    // Mark end of array with NULL info
    constant->info = NULL;
    return constant_pool;
}

class_info_t get_class_info(FILE *class_file) {
    class_info_t info;
    info.access_flags = read_u2(class_file);
    info.this_class = read_u2(class_file);
    info.super_class = read_u2(class_file);
    u2 interfaces_count = read_u2(class_file);
    assert(interfaces_count == 0 && "This VM does not support interfaces.");
    u2 fields_count = read_u2(class_file);
    assert(fields_count == 0 && "This VM does not support fields.");
    return info;
}

void read_method_attributes(FILE *class_file, method_info *info, code_t *code,
                            cp_info *constant_pool) {
    bool found_code = false;
    for (u2 attributes = info->attributes_count; attributes > 0; attributes--) {
        attribute_info ainfo;
        ainfo.attribute_name_index = read_u2(class_file);
        ainfo.attribute_length = read_u4(class_file);
        long attribute_end = ftell(class_file) + ainfo.attribute_length;
        cp_info *type_constant = get_constant(constant_pool, ainfo.attribute_name_index);
        assert(type_constant->tag == CONSTANT_Utf8 && "Expected a UTF8");
        if (strcmp(type_constant->info, "Code") == 0) {
            assert(!found_code && "Duplicate method code");
            found_code = true;

            code->max_stack = read_u2(class_file);
            code->max_locals = read_u2(class_file);
            code->code_length = read_u4(class_file);
            code->code = malloc(code->code_length);
            assert(code->code != NULL && "Failed to allocate method code");
            size_t bytes_read = fread(code->code, 1, code->code_length, class_file);
            assert(bytes_read == code->code_length && "Failed to read method code");
        }
        // Skip the rest of the attribute
        fseek(class_file, attribute_end, SEEK_SET);
    }
    assert(found_code && "Missing method code");
}

method_t *get_methods(FILE *class_file, cp_info *constant_pool) {
    u2 method_count = read_u2(class_file);
    method_t *methods = malloc(sizeof(method_t[method_count + 1]));
    assert(methods != NULL && "Failed to allocate methods");

    method_t *method = methods;
    while (method_count > 0) {
        method_info info;
        info.access_flags = read_u2(class_file);
        info.name_index = read_u2(class_file);
        info.descriptor_index = read_u2(class_file);
        info.attributes_count = read_u2(class_file);

        cp_info *name = get_constant(constant_pool, info.name_index);
        assert(name->tag == CONSTANT_Utf8 && "Expected a UTF8");
        method->name = name->info;
        cp_info *descriptor = get_constant(constant_pool, info.descriptor_index);
        assert(descriptor->tag == CONSTANT_Utf8 && "Expected a UTF8");
        method->descriptor = descriptor->info;

        /* Our JVM can only execute static methods, so ensure all methods are static.
         * However, javac creates a constructor method <init> we need to ignore. */
        if (strcmp(method->name, "<init>") != 0) {
            assert((info.access_flags & IS_STATIC) != 0 &&
                   "This VM only supports static methods.");
        }

        read_method_attributes(class_file, &info, &method->code, constant_pool);

        method++;
        method_count--;
    }

    // Mark end of array with NULL name
    method->name = NULL;
    return methods;
}

class_file_t *get_class(FILE *class_file) {
    class_file_t *class = malloc(sizeof(*class));
    assert(class != NULL && "Failed to allocate class");

    /* Read the leading header of the class file.
     * We don't need the result, but we need to skip past the header. */
    get_class_header(class_file);

    // Read the constant pool
    class->constant_pool = get_constant_pool(class_file);

    /* Read information about the class that was compiled.
     * We don't need the result, but we need to skip past it. */
    get_class_info(class_file);

    // Read the list of static methods
    class->methods = get_methods(class_file, class->constant_pool);

    return class;
}

void free_class(class_file_t *class) {
    for (cp_info *constant = class->constant_pool; constant->info != NULL; constant++) {
        free(constant->info);
    }
    free(class->constant_pool);

    for (method_t *method = class->methods; method->name != NULL; method++) {
        free(method->code.code);
    }
    free(class->methods);
    free(class);
}
