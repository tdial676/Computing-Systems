#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

#include "ast.h"

/** Parses the next statement from the provided TeenyBASIC file into an AST */
node_t *parse(FILE *stream);

#endif /* PARSER_H */
