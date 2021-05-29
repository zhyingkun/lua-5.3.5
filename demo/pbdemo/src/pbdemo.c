#include "pbc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

static void read_file(const char* filename, pbc_slice* slice) {
  FILE* f = fopen(filename, "rb");
  if (f == NULL) {
    slice->buffer = NULL;
    slice->len = 0;
    printf("Error No such file: %s\n", filename);
    return;
  }
  fseek(f, 0, SEEK_END);
  slice->len = ftell(f);
  fseek(f, 0, SEEK_SET);
  slice->buffer = malloc(slice->len);
  if (fread(slice->buffer, 1, slice->len, f) == 0) {
    printf("Error Read file failed: %s\n", filename);
    exit(1);
  }
  fclose(f);
}

static void dump(uint8_t* buffer, int sz) {
  int i, j;
  printf("dump: \n");
  for (i = 0; i < sz; i++) {
    printf("%02X ", buffer[i]);
    if (i % 16 == 15) {
      for (j = 0; j < 16; j++) {
        char c = buffer[i / 16 * 16 + j];
        if (c >= 32 && c < 127) {
          printf("%c", c);
        } else {
          printf(".");
        }
      }
      printf("\n");
    }
  }
  printf("\n");
}

void print_pbc_memory_count() {
  printf("PBC memory retain count: %d\n", pbc_memory());
}

int main() {
  pbc_slice slice;
  read_file("pb/simple.pb", &slice);
  if (slice.buffer == NULL)
    return 1;

  dump(slice.buffer, slice.len);

  print_pbc_memory_count();
  pbc_env* env = pbc_new();
  print_pbc_memory_count();
  int r = pbc_register(env, &slice);
  if (r) {
    printf("Error : %s\n", pbc_error(env));
    return 1;
  }

  free(slice.buffer);
  slice.buffer = NULL;
  slice.len = 0;

  print_pbc_memory_count();
  pbc_wmessage* msg = pbc_wmessage_new(env, "zykTest.Simple");
  pbc_wmessage_string(msg, "name", "zyk", 0);
  pbc_wmessage_integer(msg, "count", 43, 0);

  pbc_wmessage_buffer(msg, &slice);
  dump(slice.buffer, slice.len);

  pbc_rmessage* m = pbc_rmessage_new(env, "zykTest.Simple", &slice);
  if (m == NULL) {
    printf("Error : %s\n", pbc_error(env));
    return 1;
  }
  printf("name = %s\n", pbc_rmessage_string(m, "name", 0, NULL));
  printf("count = %d\n", pbc_rmessage_integer(m, "count", 0, NULL));
  pbc_rmessage_delete(m);

  pbc_wmessage_delete(msg);
  print_pbc_memory_count();

  pbc_delete(env);
  print_pbc_memory_count();
  return 0;
}
