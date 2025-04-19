#include <stdio.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// Export the function using the EMSCRIPTEN_KEEPALIVE macro
EMSCRIPTEN_KEEPALIVE
int greet(const char *name) {
  printf("Hello, %s!\n", name);
  return 0; // Return a simple value for testing
}
