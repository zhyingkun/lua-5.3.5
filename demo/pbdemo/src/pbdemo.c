#include "pbc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

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

static size_t _count = 0;

static void* realloc_fn(void* ud, void* old_ptr, size_t new_size) {
  (void)ud;
  void* new_ptr = NULL;
  if (old_ptr == NULL) { // malloc
    new_ptr = malloc(new_size);
    _count++;
  } else if (new_size == 0) { // free
    free(old_ptr);
    _count--;
  } else { // realloc
    new_ptr = realloc(old_ptr, new_size);
  }
  return new_ptr;
}
void print_pbc_memory_count() {
  printf("PBC memory retain count: %ld\n", _count);
}

int main() {
  pbc_slice pbslice;
  read_file("pb/simple.pb", &pbslice);
  if (pbslice.buffer == NULL) {
    return 1;
  }
  dump(pbslice.buffer, pbslice.len);

  pbc_set_realloc_fn(realloc_fn, NULL);
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
  pbc_wmessage_string(msg, "buffer", "TestBytes", 0);

  pbc_slice slice;
  pbc_wmessage_buffer(msg, &slice); // slice.buffer point to msg.ptr
  dump(slice.buffer, slice.len);

  char* buf_start = (char*)slice.buffer;
  char* buf_end = (char*)slice.buffer + slice.len;
  printf("slice.buffer: %p, end: %p\n", buf_start, buf_end);
  pbc_rmessage* m = pbc_rmessage_new(env, "zykTest.Simple", &slice);
  pbc_wmessage_delete(msg);
  if (m == NULL) {
    printf("Error : %s\n", pbc_error(env));
  } else {
    const char* name = pbc_rmessage_string(m, "name", 0, NULL);
    printf("name ptr: %p, IsInDeletedMemory: %s\n", name, (name >= buf_start && name < buf_end) ? "true" : "false");
    printf("name = %s\n", name);
    printf("count = %d\n", pbc_rmessage_integer(m, "count", 0, NULL));
    const char* buf = pbc_rmessage_string(m, "buffer", 0, NULL);
    printf("buffer ptr: %p, IsInDeletedMemory: %s\n", buf, (buf >= buf_start && buf < buf_end) ? "true" : "false");
    printf("buffer = %s\n", buf);
    for (int i = 0; i < slice.len; i++) {
      buf_start[i] = 'A';
    }
    printf("buffer = %s\n", buf);
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

  typedef struct {
    pbc_array name;
    int32_t count;
    pbc_slice buffer;
  } Simple;
  pbc_pattern* Simple_p = pbc_pattern_new(
      env,
      "zykTest.Simple",
      "name %a count %d buffer %s",
      offsetof(Simple, name),
      offsetof(Simple, count),
      offsetof(Simple, buffer));
  Simple myData;
  pbc_pattern_set_default(Simple_p, (void*)&myData);
  pbc_slice name1 = {"john", 5};
  pbc_slice name2 = {"tony", 5};
  pbc_array_push_slice(myData.name, &name1);
  pbc_array_push_slice(myData.name, &name2);
  myData.count = 43;
  myData.buffer.buffer = "what";
  myData.buffer.len = 5;
  // string will be deep copy to ser_slice
  int cap = 16;
  char* temp = malloc(cap);
  pbc_slice ser_slice;
  for (;;) {
    ser_slice.buffer = temp;
    ser_slice.len = cap;
    int ret = pbc_pattern_pack(Simple_p, (void*)&myData, &ser_slice);
    if (ret >= 0) {
      break;
    }
    cap = cap * 2;
    temp = realloc(temp, cap);
  }
  pbc_pattern_close_arrays(Simple_p, (void*)&myData);
  dump(ser_slice.buffer, ser_slice.len);
  Simple newData;
  pbc_pattern_unpack(Simple_p, &ser_slice, &newData);
  pbc_slice* ptr = pbc_array_slice(newData.name, 1);
  printf("name: %s, count: %d, buffer: %s\n", (char*)ptr->buffer, newData.count, (char*)newData.buffer.buffer);
  printf("name ptr: %p, buffer ptr: %p\n", ptr->buffer, newData.buffer.buffer);
  printf("temp ptr: %p, end ptr: %p\n", temp, temp + cap);
  pbc_pattern_close_arrays(Simple_p, (void*)&newData);
  free(temp);
  pbc_pattern_delete(Simple_p);

_return:
  pbc_delete(env);
  print_pbc_memory_count();
  return 0;
}
