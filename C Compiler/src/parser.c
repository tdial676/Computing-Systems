#include "parser.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// maxint = -9223372036854775808
const size_t MAX_KEYWORD_LENGTH = 100;
const value_t DEFAULT_STEP = 1;

typedef struct {
    FILE *stream;
} parser_state_t;

bool is_variable_name(char c) {
    return isupper(c);
}
bool is_open_paren(char c) {
    return c == '(';
}
bool is_close_paren(char c) {
    return c == ')';
}
bool is_factor_op(char c) {
    return c == '*' || c == '/';
}
bool is_term_op(char c) {
    return c == '+' || c == '-';
}
bool is_comparison_op(char c) {
    return c == '<' || c == '=' || c == '>';
}
bool is_operator(char c) {
    return is_open_paren(c) || is_close_paren(c) || is_factor_op(c) || is_term_op(c) ||
           is_comparison_op(c);
}
bool is_comment_start(char c) {
    return c == '#';
}

size_t save_position(parser_state_t *state) {
    return ftell(state->stream);
}

void restore_position(parser_state_t *state, size_t position) {
    fseek(state->stream, position, SEEK_SET);
}

void rewind_one(parser_state_t *state) {
    fseek(state->stream, -1, SEEK_CUR);
}

/*
 * Advances the provided state to the next token.
 */
char advance(parser_state_t *state) {
    while (true) {
        int result = fgetc(state->stream);
        if (result == EOF) {
            return '\0';
        }
        if (!isspace(result)) {
            return result;
        }
    }
}

typedef bool (*char_predicate_t)(char);

char try_advance(parser_state_t *state, char_predicate_t predicate) {
    char next = advance(state);
    if (next != '\0' && !predicate(next)) {
        rewind_one(state);
        return '\0';
    }

    return next;
}

char *advance_until_separator(parser_state_t *state) {
    char *result = malloc(sizeof(char[MAX_KEYWORD_LENGTH + 1]));
    assert(result != NULL);
    size_t index = 0;

    while (true) {
        if (index > MAX_KEYWORD_LENGTH) {
            free(result);
            return NULL;
        }
        int c = fgetc(state->stream);
        if (c == EOF) {
            if (index > 0) {
                break;
            }

            free(result);
            return NULL;
        }

        if (is_operator(c) && index > 0) {
            rewind_one(state);
            break;
        }
        if (isspace(c)) {
            if (index > 0) {
                break;
            }

            continue;
        }

        result[index++] = c;
    }
    result[index] = '\0';
    return result;
}

bool at_end(parser_state_t *state) {
    if (advance(state) != '\0') {
        rewind_one(state);
        return false;
    }

    return true;
}

void skip_line(parser_state_t *state) {
    while (true) {
        int character = fgetc(state->stream);
        if (character == EOF || character == '\n') {
            break;
        }
    }
}

node_t *num(parser_state_t *state) {
    char *num_string = advance_until_separator(state);
    if (num_string == NULL) {
        return NULL;
    }

    char *num_end;
    errno = 0;
    value_t value = strtol(num_string, &num_end, 0);
    if (errno != 0 || *num_end != '\0') {
        free(num_string);
        return NULL;
    }

    free(num_string);
    return init_num_node(value);
}

node_t *expression(parser_state_t *);

node_t *factor(parser_state_t *state) {
    if (try_advance(state, is_open_paren)) {
        node_t *node = expression(state);
        if (!try_advance(state, is_close_paren)) {
            return NULL;
        }

        return node;
    }

    char var = try_advance(state, is_variable_name);
    if (var) {
        return init_var_node(var);
    }

    return num(state);
}

node_t *term(parser_state_t *state) {
    node_t *result = factor(state);
    while (true) {
        char next = try_advance(state, is_factor_op);
        if (!next) {
            break;
        }

        result = init_binary_node(next, result, factor(state));
    }
    return result;
}

node_t *expression(parser_state_t *state) {
    node_t *result = term(state);
    while (true) {
        char next = try_advance(state, is_term_op);
        if (!next) {
            break;
        }

        result = init_binary_node(next, result, term(state));
    }
    return result;
}

binary_node_t *comparison(parser_state_t *state) {
    node_t *left = expression(state);
    char op = try_advance(state, is_comparison_op);
    return (binary_node_t *) init_binary_node(op, left, expression(state));
}

node_t *sequence(parser_state_t *);

node_t *statement(parser_state_t *state, bool *end) {
    while (try_advance(state, is_comment_start)) {
        skip_line(state);
    }

    size_t start = save_position(state);
    char *next = advance_until_separator(state);
    if (next == NULL) {
        *end = true;
        return NULL;
    }

    if (strcmp(next, "ELSE") == 0) {
        free(next);
        restore_position(state, start);
        *end = true;
        return NULL;
    }
    if (strcmp(next, "END") == 0) {
        free(next);
        next = advance_until_separator(state);
        if (next == NULL || !(strcmp(next, "IF") == 0 || strcmp(next, "WHILE") == 0)) {
            free(next);
            *end = false;
            return NULL;
        }

        free(next);
        restore_position(state, start);
        *end = true;
        return NULL;
    }

    *end = false;
    if (strcmp(next, "PRINT") == 0) {
        free(next);
        return init_print_node(expression(state));
    }
    if (strcmp(next, "LET") == 0) {
        free(next);
        char var = advance(state);
        if (!(is_variable_name(var) && advance(state) == '=')) {
            return NULL;
        }

        return init_let_node(var, expression(state));
    }
    if (strcmp(next, "IF") == 0) {
        free(next);
        binary_node_t *condition = comparison(state);
        node_t *if_branch = sequence(state);
        next = advance_until_separator(state);
        node_t *else_branch;
        if (next != NULL && strcmp(next, "ELSE") == 0) {
            free(next);
            else_branch = sequence(state);
            next = advance_until_separator(state);
        }
        else {
            else_branch = NULL;
        }
        if (next == NULL || strcmp(next, "END") != 0) {
            free(next);
            free_ast((node_t *) condition);
            free_ast(if_branch);
            free_ast(else_branch);
            return NULL;
        }

        free(next);
        next = advance_until_separator(state);
        if (next == NULL || strcmp(next, "IF") != 0) {
            free(next);
            free_ast((node_t *) condition);
            free_ast(if_branch);
            free_ast(else_branch);
            return NULL;
        }

        free(next);
        return init_if_node(condition, if_branch, else_branch);
    }
    if (strcmp(next, "WHILE") == 0) {
        free(next);
        binary_node_t *condition = comparison(state);
        node_t *body = sequence(state);
        next = advance_until_separator(state);
        if (next == NULL || strcmp(next, "END") != 0) {
            free(next);
            free_ast((node_t *) condition);
            free_ast(body);
            return NULL;
        }

        free(next);
        next = advance_until_separator(state);
        if (next == NULL || strcmp(next, "WHILE") != 0) {
            free(next);
            free_ast((node_t *) condition);
            free_ast(body);
            return NULL;
        }

        free(next);
        return init_while_node(condition, body);
    }

    free(next);
    return NULL;
}

node_t *sequence(parser_state_t *state) {
    size_t statement_count = 0;
    node_t **statements = NULL;
    size_t statement_capacity = 0;
    while (true) {
        bool end;
        node_t *next = statement(state, &end);
        if (end) {
            break;
        }
        if (next == NULL) {
            for (size_t i = 0; i < statement_count; i++) {
                free_ast(statements[i]);
            }
            free(statements);
            return NULL;
        }

        if (statement_count == statement_capacity) {
            statement_capacity = statement_capacity > 0 ? statement_capacity * 2 : 1;
            statements = realloc(statements, sizeof(node_t *[statement_capacity]));
            assert(statements != NULL);
        }
        statements[statement_count++] = next;
    }

    // Avoid allocating a sequence_node_t if there is only one statement
    if (statement_count == 1) {
        node_t *statement = *statements;
        free(statements);
        return statement;
    }

    if (statement_count > 0) {
        statements = realloc(statements, sizeof(node_t *[statement_count]));
        assert(statements != NULL);
    }
    return init_sequence_node(statement_count, statements);
}

node_t *parse(FILE *stream) {
    parser_state_t state = {.stream = stream};
    node_t *ast = sequence(&state);
    if (!at_end(&state)) {
        free_ast(ast);
        return NULL;
    }

    return ast;
}
