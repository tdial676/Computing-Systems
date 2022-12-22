#ifndef AST_H
#define AST_H

/**
 * Definitions for the abstract syntax tree representation of TeenyBASIC.
 * Parsing source code into an AST allows us to traverse it in a structured way.
 * The AST is a recursive data structure consisting of several types of "nodes".
 */

#include <inttypes.h>
#include <stddef.h>

/** The types of AST nodes */
typedef enum { NUM, BINARY_OP, VAR, SEQUENCE, PRINT, LET, IF, WHILE } node_type_t;

/** The type of a TeenyBASIC variable name */
typedef char var_name_t;
/** The type of a TeenyBASIC value */
typedef int64_t value_t;

/** The base struct for all nodes */
typedef struct {
    /**
     * The node's type. Determines how to interpret the node.
     * For example, if `node->type == BINARY_OP`,
     * then `node` can be cast to a `binary_node_t *`.
     */
    node_type_t type;
} node_t;

/** A number literal */
typedef struct {
    node_t base;
    /** The value of the literal */
    value_t value;
} num_node_t;

/**
 * An expression representing a binary operation or comparison.
 * For example, (1 + 2) would be represented as a binary_node_t with:
 *   op == '+'
 *   ((num_node_t *) left)->value == 1
 *   ((num_node_t *) right)->value == 2
 */
typedef struct {
    node_t base;
    /** The operator, either '+', '-', '*', '/', '<', '=', or '> */
    char op;
    /** The left-hand side of the expression */
    node_t *left;
    /** The right-hand side of the expression */
    node_t *right;
} binary_node_t;

/** An expression that evaluates a variable */
typedef struct {
    node_t base;
    /** The variable whose value to read ('A' to 'Z') */
    var_name_t name;
} var_node_t;

/** A sequence of statements to run in order */
typedef struct {
    node_t base;
    /** The number of statements in this sequence */
    size_t statement_count;
    /**
     * The sequence's statements.
     * This points to an array of `statement_count` pointers to statement nodes.
     */
    node_t **statements;
} sequence_node_t;

/** A PRINT statement */
typedef struct {
    node_t base;
    /** The expression to evaluate and print */
    node_t *expr;
} print_node_t;

/** A LET statement */
typedef struct {
    node_t base;
    /** The variable to assign a value to ('A' to 'Z') */
    var_name_t var;
    /** The expression to evaluate and store in the variable */
    node_t *value;
} let_node_t;

/** An IF statement */
typedef struct {
    node_t base;
    /** The condition to check, which has operator '<', '=', or '>' */
    binary_node_t *condition;
    /** The statement to run if the condition evaluates to true */
    node_t *if_branch;
    /**
     * The statement to run if the condition evaluates to false.
     * This can be NULL if the IF statement has no ELSE clause.
     */
    node_t *else_branch;
} if_node_t;

/** A WHILE statement */
typedef struct {
    node_t base;
    /** The condition to check, which has operator '<', '=', or '>' */
    binary_node_t *condition;
    /**
     * The loop body. This statement is run repeatedly
     * while `condition` evaluates to true.
     */
    node_t *body;
} while_node_t;

/** Constructs a num_node_t */
node_t *init_num_node(value_t value);

/** Constructs a binary_node_t */
node_t *init_binary_node(char op, node_t *left, node_t *right);

/** Constructs a var_node_t */
node_t *init_var_node(var_name_t name);

/** Constructs a sequence_node_t */
node_t *init_sequence_node(size_t statement_count, node_t **statements);

/** Constructs a print_node_t */
node_t *init_print_node(node_t *expr);

/** Constructs a let_node_t */
node_t *init_let_node(var_name_t var, node_t *value);

/** Constructs a cond_node_t */
node_t *init_if_node(binary_node_t *condition, node_t *if_branch, node_t *else_branch);

/** Constructs a while_node_t */
node_t *init_while_node(binary_node_t *condition, node_t *body);

/** Frees an AST node and all its descendants */
void free_ast(node_t *node);

/** Prints a string representation of an AST node to stderr */
void print_ast(node_t *node);

#endif /* AST_H */
