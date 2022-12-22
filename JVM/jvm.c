#include "jvm.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "read_class.h"

const size_t I_LOAD_IDX = 26;
const size_t I_A_LOAD_IDX = 42;
const size_t I_STORE_IDX = 59;
const size_t I_ASTORE_IDX = 75;

/** The name of the method to invoke to run the class file */
const char MAIN_METHOD[] = "main";
/**
 * The "descriptor" string for main(). The descriptor encodes main()'s signature,
 * i.e. main() takes a String[] and returns void.
 * If you're interested, the descriptor string is explained at
 * https://docs.oracle.com/javase/specs/jvms/se12/html/jvms-4.html#jvms-4.3.2.
 */
const char MAIN_DESCRIPTOR[] = "([Ljava/lang/String;)V";

/**
 * Represents the return value of a Java method: either void or an int or a reference.
 * For simplification, we represent a reference as an index into a heap-allocated array.
 * (In a real JVM, methods could also return object references or other primitives.)
 */
typedef struct {
    /** Whether this returned value is an int */
    bool has_value;
    /** The returned value (only valid if `has_value` is true) */
    int32_t value;
} optional_value_t;

/**
 * Runs a method's instructions until the method returns.
 *
 * @param method the method to run
 * @param locals the array of local variables, including the method parameters.
 *   Except for parameters, the locals are uninitialized.
 * @param class the class file the method belongs to
 * @param heap an array of heap-allocated pointers, useful for references
 * @return an optional int containing the method's return value
 */
optional_value_t execute(method_t *method, int32_t *locals, class_file_t *class,
                         heap_t *heap) {
    /* You should remove these casts to void in your solution.
     * They are just here so the code compiles without warnings. */

    int32_t *stack = calloc(method->code.max_stack, sizeof(int32_t));
    uint32_t curr = 0;
    uint32_t pc = 0;
    while (true) {
        switch (method->code.code[pc]) {
            case (i_bipush):
                stack[curr] = (int32_t)(int8_t) method->code.code[pc + 1];
                pc += 2;
                curr += 1;
                break;
            // arythmatic starts
            case (i_iadd): {
                int32_t sum = stack[curr - 1] + stack[curr - 2];
                stack[curr - 2] = sum;
                pc++;
                curr--;
                break;
            }
            case (i_isub): {
                int32_t diff = stack[curr - 2] - stack[curr - 1];
                stack[curr - 2] = diff;
                pc++;
                curr--;
                break;
            }
            case (i_imul): {
                int32_t mul = stack[curr - 1] * stack[curr - 2];
                stack[curr - 2] = mul;
                pc++;
                curr--;
                break;
            }
            case (i_idiv): {
                assert(stack[curr - 1] != 0);
                int32_t div = stack[curr - 2] / stack[curr - 1];
                stack[curr - 2] = div;
                pc++;
                curr--;
                break;
            }
            case (i_irem): {
                assert(stack[curr - 1] != 0);
                int32_t rem = stack[curr - 2] % stack[curr - 1];
                stack[curr - 2] = rem;
                pc++;
                curr--;
                break;
            }
            case (i_ineg): {
                int32_t neg = -1 * stack[curr - 1];
                stack[curr - 1] = neg;
                pc++;
                break;
            }
            case (i_ishl): {
                int32_t shl = stack[curr - 2] << stack[curr - 1];
                stack[curr - 2] = shl;
                pc++;
                curr--;
                break;
            }
            case (i_ishr): {
                int32_t shr = stack[curr - 2] >> stack[curr - 1];
                stack[curr - 2] = shr;
                pc++;
                curr--;
                break;
            }
            case (i_iushr): {
                int32_t ushr = ((uint32_t) stack[curr - 2]) >> stack[curr - 1];
                stack[curr - 2] = ushr;
                pc++;
                curr--;
                break;
            }
            case (i_iand): {
                int32_t and = stack[curr - 2] & stack[curr - 1];
                stack[curr - 2] = and;
                pc++;
                curr--;
                break;
            }
            case (i_ior): {
                int32_t or = stack[curr - 2] | stack[curr - 1];
                stack[curr - 2] = or ;
                pc++;
                curr--;
                break;
            }
            case (i_ixor): {
                int32_t xor = stack[curr - 2] ^ stack[curr - 1];
                stack[curr - 2] = xor;
                pc++;
                curr--;
                break;
            }
            // arythmatic ends

            // load start
            case (i_iload_0)...(i_iload_3):
                stack[curr] = locals[method->code.code[pc] - I_LOAD_IDX];
                pc++;
                curr += 1;
                break;
            case (i_istore_0)...(i_istore_3):
                locals[method->code.code[pc] - I_STORE_IDX] = stack[curr - 1];
                pc++;
                curr--;
                break;
            case (i_iload):
                stack[curr] = locals[method->code.code[pc + 1]];
                pc += 2;
                curr += 1;
                break;
            case (i_istore):
                locals[method->code.code[pc + 1]] = stack[curr - 1];
                pc += 2;
                curr--;
                break;
            case (i_iinc):
                locals[method->code.code[pc + 1]] +=
                    (int32_t)(int8_t) method->code.code[pc + 2];
                pc += 3;
                break;
                // Load ends

            // Jumps Start
            case (i_ifeq):
                if (stack[curr - 1] == 0) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr--;
                break;

            case (i_ifne):
                if (stack[curr - 1] != 0) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr--;
                break;

            case (i_iflt):
                if (stack[curr - 1] < 0) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr--;
                break;

            case (i_ifge):
                if (stack[curr - 1] >= 0) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr--;
                break;

            case (i_ifgt):
                if (stack[curr - 1] > 0) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr--;
                break;

            case (i_ifle):
                if (stack[curr - 1] <= 0) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr--;
                break;

            case (i_if_icmpeq):
                if (stack[curr - 2] == stack[curr - 1]) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr -= 2;
                break;

            case (i_if_icmpne):
                if (stack[curr - 2] != stack[curr - 1]) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr -= 2;
                break;

            case (i_if_icmplt):
                if (stack[curr - 2] < stack[curr - 1]) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr -= 2;
                break;

            case (i_if_icmpge):
                if (stack[curr - 2] >= stack[curr - 1]) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr -= 2;
                break;

            case (i_if_icmpgt):
                if (stack[curr - 2] > stack[curr - 1]) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr -= 2;
                break;

            case (i_if_icmple):
                if (stack[curr - 2] <= stack[curr - 1]) {
                    pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                    (uint8_t) method->code.code[pc + 2]);
                }
                else {
                    pc += 3;
                }
                curr -= 2;
                break;

            case (i_goto):
                pc += (int16_t)((method->code.code[pc + 1] << 8) |
                                (uint8_t) method->code.code[pc + 2]);
                break;

                // Jumps End

            case (i_ldc):
                stack[curr] =
                    *(int32_t *) class->constant_pool[method->code.code[pc + 1] - 1].info;
                pc += 2;
                curr += 1;
                break;

            case (i_getstatic):
                pc += 3;
                break;

            case (i_invokevirtual):
                printf("%d\n", stack[curr - 1]);
                curr--;
                pc += 3;
                break;

            case (i_iconst_m1)...(i_iconst_5):
                stack[curr] = method->code.code[pc] - 0x3;
                curr += 1;
                pc++;
                break;

            case (i_sipush):
                stack[curr] = (int16_t)((method->code.code[pc + 1] << 8) |
                                        (uint8_t) method->code.code[pc + 2]);
                curr += 1;
                pc += 3;
                break;

            // Task 8
            case (i_return): {
                optional_value_t result = {.has_value = false};
                free(stack);
                return result;
            }
            case (i_ireturn): {
                optional_value_t result = {.has_value = true, .value = stack[curr - 1]};
                free(stack);
                return result;
            }
            case (i_invokestatic): {
                uint16_t idx = ((method->code.code[pc + 1] << 8) |
                                (uint8_t) method->code.code[pc + 2]);
                method_t *method_location = find_method_from_index(idx, class);
                int32_t *new_locals =
                    calloc(sizeof(int32_t), method_location->code.max_locals);
                size_t paramters = get_number_of_parameters(method_location);
                for (size_t i = 0; i < paramters; i++) {
                    curr--;
                    new_locals[paramters - 1 - i] = stack[curr];
                };
                optional_value_t result =
                    execute(method_location, new_locals, class, heap);
                if (result.has_value) {
                    stack[curr] = result.value;
                    curr += 1;
                };
                pc += 3;
                free(new_locals);
                break;
            }

            // Task9
            case (i_nop):
                pc++;
                break;

            case (i_dup):
                stack[curr] = stack[curr - 1];
                curr += 1;
                pc++;
                break;

            case (i_newarray): {
                int32_t *to_add = calloc(sizeof(int32_t), stack[curr - 1] + 1);
                to_add[0] = stack[curr - 1];
                stack[curr - 1] = heap_add(heap, to_add);
                pc += 2;
                break;
            }

            case (i_arraylength): {
                int32_t len = (heap_get(heap, stack[curr - 1]))[0];
                stack[curr - 1] = len;
                pc++;
                break;
            }

            case (i_areturn): {
                optional_value_t result = {.has_value = true, .value = stack[curr - 1]};
                curr--;
                pc++;
                free(stack);
                return result;
            }

            case (i_iastore): {
                int32_t *array = heap_get(heap, stack[curr - 3]);
                array[stack[curr - 2] + 1] = stack[curr - 1];
                curr -= 3;
                pc++;
                break;
            }

            case (i_iaload): {
                int32_t *array = heap_get(heap, stack[curr - 2]);
                stack[curr - 2] = array[stack[curr - 1] + 1];
                curr--;
                pc++;
                break;
            }

            case (i_aload):
                stack[curr] = locals[method->code.code[pc + 1]];
                pc += 2;
                curr += 1;
                break;

            case (i_astore):
                locals[method->code.code[pc + 1]] = stack[curr - 1];
                pc += 2;
                curr--;
                break;

            case (i_aload_0)...(i_aload_3):
                stack[curr] = locals[method->code.code[pc] - I_A_LOAD_IDX];
                curr += 1;
                pc++;
                break;

            case (i_astore_0)...(i_astore_3):
                locals[method->code.code[pc] - I_ASTORE_IDX] = stack[curr - 1];
                curr--;
                pc++;
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s <class file>\n", argv[0]);
        return 1;
    }

    // Open the class file for reading
    FILE *class_file = fopen(argv[1], "r");
    assert(class_file != NULL && "Failed to open file");

    // Parse the class file
    class_file_t *class = get_class(class_file);
    int error = fclose(class_file);
    assert(error == 0 && "Failed to close file");

    // The heap array is initially allocated to hold zero elements.
    heap_t *heap = heap_init();

    // Execute the main method
    method_t *main_method = find_method(MAIN_METHOD, MAIN_DESCRIPTOR, class);
    assert(main_method != NULL && "Missing main() method");
    /* In a real JVM, locals[0] would contain a reference to String[] args.
     * But since TeenyJVM doesn't support Objects, we leave it uninitialized. */
    int32_t locals[main_method->code.max_locals];
    // Initialize all local variables to 0
    memset(locals, 0, sizeof(locals));
    optional_value_t result = execute(main_method, locals, class, heap);
    assert(!result.has_value && "main() should return void");

    // Free the internal data structures
    free_class(class);

    // Free the heap
    heap_free(heap);
}
