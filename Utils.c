//
// Created by kir on 09.05.2025.
//
#include "Utils.h"

#include <stdio.h>
#include <string.h>

void build_expression_from_argv(char *buffer, const size_t buffer_size,
                                const int argc, const char **argv) {
  if (buffer == NULL || buffer_size == 0 || argc == 0 || argv == NULL) {
    if (buffer_size > 0 && buffer != NULL) {
      buffer[0] = '\0';
    }
    return;
  }

  size_t pos = 0;

  for (int i = 0; i < argc; i++) {
    const char *arg = argv[i];
    const size_t arg_len = strlen(arg);
    const int add_space = (i != argc - 1);
    const size_t required_space = arg_len + (add_space ? 1 : 0);

    if (pos + required_space >= buffer_size) {
      if (pos == 0 && arg_len >= buffer_size) {
        strncpy(buffer, arg, buffer_size - 1);
        pos = buffer_size - 1;
      }
      break;
    }

    strncpy(buffer + pos, arg, arg_len);
    pos += arg_len;

    if (add_space) {
      buffer[pos] = ' ';
      pos++;
    }
  }

  buffer[(pos < buffer_size) ? pos : buffer_size - 1] = '\0';
}

ErrorType input(char *buffer, const int buffer_size, const char *prompt) {
  printf("%s", prompt);
  if (fgets(buffer, buffer_size, stdin) == NULL) {
    return ERROR_TYPE_INPUT_FAILED;
  }
  buffer[strcspn(buffer, "\n")] = '\0'; // Удаление '\n'
  if (strlen(buffer) == buffer_size - 1)
    while (getchar() != '\n') {
    }

  return ERROR_TYPE_OK;
}
