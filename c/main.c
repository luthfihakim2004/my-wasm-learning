#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

char *saved_input = NULL;

// Export the function using the EMSCRIPTEN_KEEPALIVE macro
EMSCRIPTEN_KEEPALIVE
void store_input(const char *input) {
  if (saved_input) {
    free(saved_input); // free old memory
  }
  saved_input = strdup(input); // allocate and copy input
  printf("Stored input: %s\n", saved_input);
}

EMSCRIPTEN_KEEPALIVE
const char *get_input() {
  return saved_input ? saved_input : "(no input stored)";
}
