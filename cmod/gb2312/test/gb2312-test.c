#include <stdio.h>
#include <assert.h>
#include <gb2312.h>

int main(int argc, const char* argv[]) {
  (void)argc;
  (void)argv;
  unsigned int gbcode = 0;
  size_t nullcount = 0;
  size_t validcount = 0;
  size_t symcount = 0;
  size_t chinese = 0;
  // First byte: [0xA1, 0xA9] and [0xB0, 0xF7], Second byte: [0xA1, 0xFE]
  // 0xA1 => 0
  // 0xA9 => 8
  for (size_t i = 0; i < 94 - 7 - 6; i++) {
    for (size_t j = 0; j < 94; j++) {
      size_t idx = i * 94 + j;
      if (GB2312_Unicode[idx].unicode == 0x0000) {
        nullcount++;
      } else {
        validcount++;
        if (i < 9) {
          symcount++;
        } else {
          chinese++;
        }
      }
      if (j == 0) {
        gbcode = GB2312_Unicode[idx].gb2312;
        unsigned int section = i + (i > 8 ? 6 : 0) + 0xA1;
        if (gbcode != ((section << 8) | 0xA1)) {
          fprintf(stderr, "Error => gcode: %u, section: %u, bit: 0xA1\n", gbcode, section);
          return -1;
        }
        continue;
      }
      unsigned int next = GB2312_Unicode[idx].gb2312;
      if (next != gbcode + 1) {
        fprintf(stderr, "Error: Should be continuation in One Section!\n");
        return -1;
      }
      gbcode = next;
    }
  }
  assert(GB2312_Unicode_Size == 81 * 94);
  assert(nullcount + validcount == GB2312_Unicode_Size);
  assert(symcount + chinese == validcount);
  printf("All: %zu, Valid: %zu, NULL: %zu, Symbol: %zu, Chinese: %zu\n",
         GB2312_Unicode_Size,
         validcount,
         nullcount,
         symcount,
         chinese);
  printf("GB2312 map is OK!\n");
  return 0;
}
