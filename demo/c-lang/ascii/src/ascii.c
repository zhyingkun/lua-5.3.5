#include <stdio.h>
#include <assert.h>

#include <lctype.c>

int main(int argc, const char* argv[]) {
  (void)argc;
  (void)argv;
  printf("sizeof(luai_ctype_): %lu\n", sizeof(luai_ctype_));
  //   for (int i = 0; i < 256; i++) {
  //     printf("%c, %d, is printable: %s\n", (char)i, i, lisprint(i) ? "true" : "false");
  //   }
  int count = 0;
  int nop2p = 0;
  int p2nop = 0;
  for (int i = 0; i < 256; i++) {
    if (!nop2p && lisprint(i)) {
      printf("Not printable to printable: %d\n", i);
      nop2p = 1;
    }
    if (nop2p && !p2nop && !lisprint(i)) {
      printf("\nPrintable to not printable: %d\n", i);
      p2nop = 1;
    }
    if (lisprint(i)) {
      assert(nop2p && !p2nop);
      printf("%c", i);
      count++;
      if (count % 8 == 0) {
        printf("\n");
      }
    } else {
      assert(!nop2p || p2nop);
    }
  }
  printf("count: %d\n", count);
  char arr[] = {
      '\0',
      '\a',
      '\b',
      '\t',
      '\n',
      '\v',
      '\f',
      '\r',
  };
  for (size_t i = 0; i < sizeof(arr); i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");
  return 0;
}
