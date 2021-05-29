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
  if (slice->buffer == NULL) {
    printf("Error Malloc memory for file: %s\n", filename);
  } else if (fread(slice->buffer, 1, slice->len, f) == 0) {
    free(slice->buffer);
    slice->buffer = NULL;
    slice->len = 0;
    printf("Error Read file failed: %s\n", filename);
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
  pbc_slice pbslice;
  read_file("pb/simple.pb", &pbslice);
  if (pbslice.buffer == NULL) {
    return 1;
  }
  dump(pbslice.buffer, pbslice.len);

  print_pbc_memory_count();
  pbc_env* env = pbc_new();
  print_pbc_memory_count();
  int r = pbc_register(env, &pbslice);
  if (r) {
    printf("Error : %s\n", pbc_error(env));
    goto _return;
  }

  free(pbslice.buffer);
  pbslice.buffer = NULL;
  pbslice.len = 0;

  print_pbc_memory_count();
  pbc_wmessage* msg = pbc_wmessage_new(env, "zykTest.Simple");
  pbc_wmessage_string(msg, "name", "zyk", 0);
  pbc_wmessage_integer(msg, "count", 43, 0);

  pbc_slice slice;
  pbc_wmessage_buffer(msg, &slice); // slice.buffer point to msg.ptr
  dump(slice.buffer, slice.len);

  pbc_rmessage* m = pbc_rmessage_new(env, "zykTest.Simple", &slice);
  pbc_wmessage_delete(msg);
  if (m == NULL) {
    printf("Error : %s\n", pbc_error(env));
  } else {
    printf("name = %s\n", pbc_rmessage_string(m, "name", 0, NULL));
    printf("count = %d\n", pbc_rmessage_integer(m, "count", 0, NULL));
    pbc_rmessage_delete(m);
  }
  print_pbc_memory_count();

  pbc_slice wdslice;
  //    read_file("wdbin/simple_right.wdbin", &wdslice);
  read_file("wdbin/simple_err.wdbin", &wdslice);
  if (wdslice.buffer == NULL) {
    goto _return;
  }
  pbc_rmessage* wdm = pbc_rmessage_new(env, "zykTest.Simple", &wdslice);
  free(wdslice.buffer);
  wdslice.buffer = NULL;
  wdslice.len = 0;
  if (wdm == NULL) {
    printf("Error : %s\n", pbc_error(env));
  } else {
    printf("name = %s\n", pbc_rmessage_string(wdm, "name", 0, NULL));
    printf("count = %d\n", pbc_rmessage_integer(wdm, "count", 0, NULL));
    pbc_rmessage_delete(wdm);
  }

_return:
  pbc_delete(env);
  print_pbc_memory_count();
  return 0;
}
