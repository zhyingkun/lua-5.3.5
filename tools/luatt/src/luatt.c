#include <stdio.h>
#include <stdlib.h>

static void usage(char* cmdName) {
  printf("\tusage: %s <TagTypeNumber>\n\tTagTypeNumber: Integer Number\n", cmdName);
}

/*
** basic types
*/
#define LUA_TNONE (-1)

#define LUA_TNIL 0
#define LUA_TBOOLEAN 1
#define LUA_TLIGHTUSERDATA 2
#define LUA_TNUMBER 3
#define LUA_TSTRING 4
#define LUA_TTABLE 5
#define LUA_TFUNCTION 6
#define LUA_TUSERDATA 7
#define LUA_TTHREAD 8
// Number of Tags
#define LUA_NUMTAGS 9
#define LUA_TPROTO LUA_NUMTAGS
#define LUA_TDEADKEY (LUA_NUMTAGS + 1)

static const char* const ActualTagNames[] = {
    "LUA_TNIL",
    "LUA_TBOOLEAN",
    "LUA_TLIGHTUSERDATA",
    "LUA_TNUMBER",
    "LUA_TSTRING",
    "LUA_TTABLE",
    "LUA_TFUNCTION",
    "LUA_TUSERDATA",
    "LUA_TTHREAD",
    "LUA_TPROTO",
    "LUA_TDEADKEY",
};
static const char* const VariantTagFunctions[3] = {
    "Lua Closure",
    "Light C Function",
    "C Closure",
};
static const char* const VariantTagStrings[2] = {
    "Short Strings",
    "Long Strings",
};
static const char* const VariantTagNumbers[2] = {
    "Float Numbers",
    "Integer Numbers",
};
static const char* const IsCollectable[2] = {
    "Not Collectable",
    "Collectable",
};
static const char* const TagTypeNone = "LUA_TNONE";
static const int AllTypeNumber[] = {
    0, // nil
    1, // boolean
    2, // light userdata
    3, // float number
    3 + 1 * 16, // integer number
    4 + 0 * 16 + 1 * 64, // short string
    4 + 1 * 16 + 1 * 64, // long string
    5 + 0 * 16 + 1 * 64, // table
    6 + 0 * 16 + 1 * 64, // lua closure
    6 + 1 * 16, // light c function
    6 + 2 * 16 + 1 * 64, // c closure
    7 + 0 * 16 + 1 * 64, // userdata
    8 + 0 * 16 + 1 * 64, // thread
    9, // proto
    10, // deadkey
    -1, // none
};

static int is_type_num_valid(int type) {
  int length = sizeof(AllTypeNumber) / sizeof(int);
  for (int i = 0; i < length; i++) {
    if (type == AllTypeNumber[i]) {
      return 1;
    }
  }
  return 0;
}

static void display_type(int tagType) {
  if (tagType == -1) {
    printf("%3d => ActualTag: %s\n", tagType, TagTypeNone);
    return;
  }
  int actualTag = tagType & 0x0f;
  int variantBits = (tagType >> 4) & 0x03;
  int collectable = (tagType >> 6) & 0x01;
  printf(
      "%3d => %2d %2d %2d => ActualTag: %s", tagType, collectable, variantBits, actualTag, ActualTagNames[actualTag]);
  switch (actualTag) {
    case LUA_TFUNCTION:
      printf(" VariantTag: %s", VariantTagFunctions[variantBits]);
      break;
    case LUA_TSTRING:
      printf(" VariantTag: %s", VariantTagStrings[variantBits]);
      break;
    case LUA_TNUMBER:
      printf(" VariantTag: %s", VariantTagNumbers[variantBits]);
      break;
    default:
      break;
  }
  if (collectable == 1) {
    printf(" [%s]", IsCollectable[collectable]);
  }
  printf("\n");
}

static void display_all_type() {
  int length = sizeof(AllTypeNumber) / sizeof(int);
  for (int i = 0; i < length; i++) {
    display_type(AllTypeNumber[i]);
  }
}
int main(int argc, char* argv[]) {
  if (argc != 2) {
    usage(argv[0]);
    display_all_type();
    return 0;
  }
  int tagType = atoi(argv[1]);
  if (is_type_num_valid(tagType) == 0) {
    fprintf(stderr, "Tag type %d is invalid!\n", tagType);
    return 0;
  }
  display_type(tagType);
  return 0;
}
