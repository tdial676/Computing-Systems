#include "compile.h"

#include <stdio.h>
#include <stdlib.h>

size_t IF_COUNT = 0;
size_t WHILE_COUNT = 0;

void comp_num(node_t *node) {
    num_node_t *num = (num_node_t *) node;
    printf("mov $%ld, %s\n", num->value, "%rdi");
}

void comp_print(node_t *node) {
    print_node_t *purr = (print_node_t *) node;
    compile_ast(purr->expr);
    printf("%s\n", "callq print_int");
}

void comp_seq(node_t *node) {
    sequence_node_t *seq = (sequence_node_t *) node;
    for (size_t i = 0; i < seq->statement_count; i++) {
        compile_ast(seq->statements[i]);
    }
}

void comp_read(node_t *node) {
    var_node_t *read = (var_node_t *) node;
    printf("mov %d(%s), %s\n", -((read->name - 64) * 8), "%rbp", "%rdi");
}

void comp_write(node_t *node) {
    let_node_t *wrt = (let_node_t *) node;
    compile_ast(wrt->value);
    printf("mov %s, %d(%s)\n", "%rdi", -((wrt->var - 64) * 8), "%rbp");
}

void comp_if(node_t *node) {
    IF_COUNT += 1;
    size_t count = IF_COUNT;
    if_node_t *cond = (if_node_t *) node;
    compile_ast(cond->condition->left);
    printf("push %s\n", "%rdi");
    compile_ast(cond->condition->right);
    printf("pop %s\n", "%rax");
    switch (cond->condition->op) {
        case ('<'):
            printf("cmp %s, %s\n", "%rdi", "%rax");
            printf("js IF_BRANCH_%zu\n", count);
            break;
        case ('>'):
            printf("cmp %s, %s\n", "%rax", "%rdi");
            printf("js IF_BRANCH_%zu\n", count);
            break;
        case ('='):
            printf("cmp %s, %s\n", "%rdi", "%rax");
            printf("je IF_BRANCH_%zu\n", count);
            break;
        default:
            break;
    }
    if (cond->else_branch != NULL) {
        compile_ast(cond->else_branch);
    }
    printf("jmp IF_END_%zu\n", count);
    printf("IF_BRANCH_%zu:\n", count);
    compile_ast(cond->if_branch);
    printf("IF_END_%zu:\n", count);
}

// Struct that allows us to simultaniously check if
// the tree is collapsable and return its value.
typedef struct collapse {
    bool can;
    value_t value;
} collapse_t;

collapse_t collapse_helper(node_t *node) {
    switch (node->type) {
        collapse_t result;
        case (NUM): {
            num_node_t *num = (num_node_t *) node;
            result.can = true;
            result.value = num->value;
            return result;
        }
        case (BINARY_OP): {
            binary_node_t *bin = (binary_node_t *) node;
            collapse_t left = collapse_helper(bin->left);
            collapse_t right = collapse_helper(bin->right);
            if (!left.can || !right.can) {
                result.can = false;
                result.value = 0;
                return result;
            }
            switch (bin->op) {
                case ('-'):
                    result.can = true;
                    result.value = left.value - right.value;
                    return (result);
                case ('*'):
                    result.can = true;
                    result.value = left.value * right.value;
                    return (result);
                case ('+'):
                    result.can = true;
                    result.value = left.value + right.value;
                    return (result);
                case ('/'):
                    result.can = true;
                    result.value = left.value / right.value;
                    return (result);
                default:
                    break;
            }
        }
        default: {
            result.can = false;
            result.value = 0;
            return result;
        }
    }
}

void comp_while(node_t *node) {
    while_node_t *cond = (while_node_t *) node;
    WHILE_COUNT += 1;
    size_t count = WHILE_COUNT;
    printf("WHILE_LABEL_%zu:\n", count);
    compile_ast(cond->condition->left);
    printf("push %s\n", "%rdi");
    compile_ast(cond->condition->right);
    printf("pop %s\n", "%rax");
    switch (cond->condition->op) {
        case ('>'):
            printf("cmp %s, %s\n", "%rax", "%rdi");
            printf("jns WHILE_END_%zu\n", count);
            break;
        case ('<'):
            printf("cmp %s, %s\n", "%rdi", "%rax");
            printf("jns WHILE_END_%zu\n", count);
            break;
        case ('='):
            printf("cmp %s, %s\n", "%rdi", "%rax");
            printf("jne WHILE_END_%zu\n", count);
            break;
        default:
            break;
    }
    compile_ast(cond->body);
    printf("jmp WHILE_LABEL_%zu\n", count);
    printf("WHILE_END_%zu:\n", count);
}

// Helper to get the power of two of a number
int get_power(value_t num) {
    int pow = 0;
    while (num > 1) {
        num = num / 2;
        pow += 1;
    }
    return pow;
}

void comp_bin(node_t *node) {
    binary_node_t *bin = (binary_node_t *) node;
    compile_ast(bin->left);
    if (bin->op != '*') {
        printf("push %s\n", "%rdi");
        compile_ast(bin->right);
        printf("pop %s\n", "%rax");
    }
    switch (bin->op) {
        case ('+'):
            printf("addq %s, %s\n", "%rax", "%rdi");
            break;
        case ('*'):
            if (bin->right->type == NUM) {
                num_node_t *num = (num_node_t *) bin->right;
                if ((num->value & (num->value - 1)) == 0 && (num->value) > 0) {
                    int n = get_power(num->value);
                    printf("shl $%d, %s\n", n, "%rdi");
                    break;
                }
            }
            printf("push %s\n", "%rdi");
            compile_ast(bin->right);
            printf("pop %s\n", "%rax");
            printf("imulq %s, %s\n", "%rax", "%rdi");
            break;
        case ('-'):
            printf("subq %s, %s\n", "%rdi", "%rax");
            printf("mov %s, %s\n", "%rax", "%rdi");
            break;
        case ('/'):
            printf("cqto\n");
            printf("idiv %s\n", "%rdi");
            printf("mov %s, %s\n", "%rax", "%rdi");
            break;
        default:
            break;
    }
}

bool compile_ast(node_t *node) {
    switch (node->type) {
        case (NUM):
            comp_num(node);
            break;
        case (PRINT):
            comp_print(node);
            break;
        case (SEQUENCE):
            comp_seq(node);
            break;

        case (BINARY_OP): {
            collapse_t result = collapse_helper(node);
            if (!result.can) {
                comp_bin(node);
                break;
            }
            fprintf(stderr, "%ldd", result.value);
            printf("mov $%ld, %s\n", result.value, "%rdi");
            break;
        }
        case (VAR):
            comp_read(node);
            break;
        case (LET):
            comp_write(node);
            break;
        case (IF):
            comp_if(node);
            break;
        case (WHILE):
            comp_while(node);
            break;
        default:
            break;
    }
    return true;
}
