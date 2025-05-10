//
// Created by kir on 09.05.2025.
//

#ifndef UTILS_H
#define UTILS_H
#include "Enums.h"

#include <stddef.h>

void build_expression_from_argv(char *buffer, size_t buffer_size, int argc,
                                const char **argv);

ErrorType input(char *buffer, int buffer_size, const char *prompt);

#endif // UTILS_H
