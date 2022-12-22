#ifndef CLASS_FILE_H
#define CLASS_FILE_H

/*
 * Definitions for pieces of a Java class file. If you're interested,
 * https://docs.oracle.com/javase/specs/jvms/se12/html/jvms-4.html
 * has the complete specification.
 *
 * Since some of these structs correspond to structs in the JVM specification,
 * we use the same names although they don't follow the code quality guidelines.
 */

#include <inttypes.h>

/* Integer type aliases used in the JVM documentation.
 * You may use these aliases or the corresponding inttypes.h types. */
typedef uint8_t u1;
typedef uint16_t u2;
typedef uint32_t u4;

typedef struct {
    u4 magic;
    u2 minor_version;
    u2 major_version;
} class_header_t;

typedef struct {
    u2 access_flags;
    u2 this_class;
    u2 super_class;
} class_info_t;

typedef struct {
    u2 access_flags;
    u2 name_index;
    u2 descriptor_index;
    u2 attributes_count;
} method_info;

typedef struct {
    u2 attribute_name_index;
    u4 attribute_length;
} attribute_info;

/** The JVM's representation of a Java method's code */
typedef struct {
    /** The maximum number of ints that will be on the operand stack */
    u2 max_stack;
    /** The number of int locals (method parameters + local variables) */
    u2 max_locals;
    /** The number of bytes in the method's bytecode */
    u4 code_length;
    /**
     * The method's bytecode, a list of JVM instructions represented as bytes.
     * See the project01 spec for how to interpret these bytes.
     */
    u1 *code;
} code_t;

/** A Java method */
typedef struct {
    /**
     * The method name, e.g. "main".
     * This is used with the descriptor string to look up the method.
     */
    char *name;
    /**
     * The method descriptor, e.g. "([Ljava/lang/String;)V",
     * which represents the method's signature.
     * This is used together with the name to look up the method,
     * which allows for method overloading.
     * If you're interested, descriptor strings are explained at
     * https://docs.oracle.com/javase/specs/jvms/se12/html/jvms-4.html#jvms-4.3.2.
     */
    char *descriptor;
    /** The method's bytecode (see the comments for `code_t`) */
    code_t code;
} method_t;

/**
 * Magic numbers the JVM uses to identify the types of constant pool entry.
 * You will only need to handle the CONSTANT_Integer case.
 */
typedef enum {
    CONSTANT_Utf8 = 1,
    CONSTANT_Integer = 3,
    CONSTANT_Class = 7,
    CONSTANT_Fieldref = 9,
    CONSTANT_Methodref = 10,
    CONSTANT_NameAndType = 12
} cp_tag_t;

typedef struct {
    u2 string_index;
} CONSTANT_Class_info;

typedef struct {
    u2 class_index;
    u2 name_and_type_index;
} CONSTANT_FieldOrMethodref_info;

/** The representation of an int in a constant pool entry */
typedef struct {
    int32_t bytes;
} CONSTANT_Integer_info;

typedef struct {
    u2 name_index;
    u2 descriptor_index;
} CONSTANT_NameAndType_info;

/** An entry in a class file's constant pool */
typedef struct {
    /** The type of constant, which determines how to interpret `info` */
    cp_tag_t tag;
    /**
     * A pointer to the constant's value.
     * `tag` determines what type of value `info` points to.
     * For example, an integer constant's `info` points to a CONSTANT_Integer_info struct.
     */
    void *info;
} cp_info;

/** A class file, consisting of an array of constants and an array of methods */
typedef struct {
    /**
     * The class's array of constants.
     * Note that this array is 0-indexed, but the bytecode refers to 1-indexed constants.
     * The array is "null-terminated": `constant_pool[length].info == NULL`.
     */
    cp_info *constant_pool;
    /**
     * The class's methods, in no particular order.
     * The array is "null-terminated": `methods[length].name == NULL`.
     */
    method_t *methods;
} class_file_t;

#endif /* CLASS_FILE_H */
