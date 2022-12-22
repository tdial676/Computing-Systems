#include "ast.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

node_t *init_num_node(value_t value) {
    num_node_t *node = malloc(sizeof(num_node_t));
    assert(node != NULL);
    node->base.type = NUM;
    node->value = value;
    return (node_t *) node;
}

node_t *init_binary_node(char op, node_t *left, node_t *right) {
    if (left == NULL || right == NULL) {
        free_ast(left);
        free_ast(right);
        return NULL;
    }

    binary_node_t *node = malloc(sizeof(binary_node_t));
    assert(node != NULL);
    node->base.type = BINARY_OP;
    node->op = op;
    node->left = left;
    node->right = right;
    return (node_t *) node;
}

node_t *init_var_node(var_name_t name) {
    if (name == '\0') {
        return NULL;
    }

    var_node_t *node = malloc(sizeof(var_node_t));
    assert(node != NULL);
    node->base.type = VAR;
    node->name = name;
    return (node_t *) node;
}

node_t *init_sequence_node(size_t statement_count, node_t **statements) {
    if (statements == NULL && statement_count > 0) {
        return NULL;
    }

    sequence_node_t *node = malloc(sizeof(sequence_node_t));
    assert(node != NULL);
    node->base.type = SEQUENCE;
    node->statement_count = statement_count;
    node->statements = statements;
    return (node_t *) node;
}

node_t *init_print_node(node_t *expr) {
    if (expr == NULL) {
        return NULL;
    }

    print_node_t *node = malloc(sizeof(print_node_t));
    assert(node != NULL);
    node->base.type = PRINT;
    node->expr = expr;
    return (node_t *) node;
}

node_t *init_let_node(var_name_t var, node_t *value) {
    if (var == '\0' || value == NULL) {
        free_ast(value);
        return NULL;
    }

    let_node_t *node = malloc(sizeof(let_node_t));
    assert(node != NULL);
    node->base.type = LET;
    node->var = var;
    node->value = value;
    return (node_t *) node;
}

node_t *init_if_node(binary_node_t *condition, node_t *if_branch, node_t *else_branch) {
    if (condition == NULL || if_branch == NULL) {
        free_ast((node_t *) condition);
        free_ast(if_branch);
        free_ast(else_branch);
        return NULL;
    }

    if_node_t *node = malloc(sizeof(if_node_t));
    assert(node != NULL);
    node->base.type = IF;
    node->condition = condition;
    node->if_branch = if_branch;
    node->else_branch = else_branch;
    return (node_t *) node;
}

node_t *init_while_node(binary_node_t *condition, node_t *body) {
    if (condition == NULL || body == NULL) {
        free_ast((node_t *) condition);
        free_ast(body);
        return NULL;
    }

    while_node_t *node = malloc(sizeof(while_node_t));
    assert(node != NULL);
    node->base.type = WHILE;
    node->condition = condition;
    node->body = body;
    return (node_t *) node;
}

void free_ast(node_t *node) {
    if (node == NULL) {
        return;
    }

    if (node->type == BINARY_OP) {
        binary_node_t *bin = (binary_node_t *) node;
        free_ast(bin->left);
        free_ast(bin->right);
    }
    else if (node->type == SEQUENCE) {
        sequence_node_t *sequence = (sequence_node_t *) node;
        for (size_t i = 0; i < sequence->statement_count; i++) {
            free_ast(sequence->statements[i]);
        }
        free(sequence->statements);
    }
    else if (node->type == PRINT) {
        free_ast(((print_node_t *) node)->expr);
    }
    else if (node->type == LET) {
        free_ast(((let_node_t *) node)->value);
    }
    else if (node->type == IF) {
        if_node_t *conditional = (if_node_t *) node;
        free_ast((node_t *) conditional->condition);
        free_ast(conditional->if_branch);
        free_ast(conditional->else_branch);
    }
    else if (node->type == WHILE) {
        while_node_t *loop = (while_node_t *) node;
        free_ast((node_t *) loop->condition);
        free_ast(loop->body);
    }
    free(node);
}

void print_indent(size_t indent) {
    while (indent > 0) {
        fprintf(stderr, "\t");
        indent--;
    }
}

void print_ast_indented(node_t *node, size_t indent) {
    if (node->type == NUM) {
        fprintf(stderr, "%" PRId64, ((num_node_t *) node)->value);
    }
    else if (node->type == BINARY_OP) {
        binary_node_t *bin = (binary_node_t *) node;
        fprintf(stderr, "%c(", bin->op);
        print_ast_indented(bin->left, indent);
        fprintf(stderr, ", ");
        print_ast_indented(bin->right, indent);
        fprintf(stderr, ")");
    }
    else if (node->type == VAR) {
        fprintf(stderr, "%c", ((var_node_t *) node)->name);
    }
    else if (node->type == SEQUENCE) {
        sequence_node_t *sequence = (sequence_node_t *) node;
        for (size_t i = 0; i < sequence->statement_count; i++) {
            print_ast_indented(sequence->statements[i], indent);
        }
    }
    else if (node->type == PRINT) {
        print_indent(indent);
        fprintf(stderr, "PRINT(");
        print_ast_indented(((print_node_t *) node)->expr, indent);
        fprintf(stderr, ")\n");
    }
    else if (node->type == LET) {
        print_indent(indent);
        let_node_t *let = (let_node_t *) node;
        fprintf(stderr, "LET(%c, ", let->var);
        print_ast_indented(let->value, indent);
        fprintf(stderr, ")\n");
    }
    else if (node->type == IF) {
        if_node_t *conditional = (if_node_t *) node;
        print_indent(indent);
        fprintf(stderr, "IF(");
        print_ast_indented((node_t *) conditional->condition, indent);
        fprintf(stderr, ",\n");
        print_ast_indented(conditional->if_branch, indent + 1);
        if (conditional->else_branch != NULL) {
            print_indent(indent);
            fprintf(stderr, ",\n");
            print_ast_indented(conditional->else_branch, indent + 1);
        }
        print_indent(indent);
        fprintf(stderr, ")\n");
    }
    else if (node->type == WHILE) {
        while_node_t *loop = (while_node_t *) node;
        print_indent(indent);
        fprintf(stderr, "WHILE(");
        print_ast_indented((node_t *) loop->condition, indent);
        fprintf(stderr, ",\n");
        print_ast_indented(loop->body, indent + 1);
        print_indent(indent);
        fprintf(stderr, ")\n");
    }
    else {
        fprintf(stderr, "\nUnknown node type: %d\n", node->type);
        assert(false);
    }
}

void print_ast(node_t *node) {
    print_ast_indented(node, 0);
}
