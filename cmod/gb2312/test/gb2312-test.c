#include <stdio.h>
#include <assert.h>
#include <gb2312.h>

int main(int argc, const char* argv[]) {
  (void)argc;
  (void)argv;
  unsigned int gbcode = 0;
  for (size_t i = 0; i < 94 - 7 - 6; i++) {
    for (size_t j = 0; j < 94; j++) {
      if (j == 0) {
        gbcode = GB2312_Unicode[i * 94 + j].gb2312;
        continue;
      }
      unsigned int next = GB2312_Unicode[i * 94 + j].gb2312;
      assert(next == gbcode + 1);
      gbcode = next;
    }
  }
  printf("GB2312 map is OK!\n");
  return 0;
}
