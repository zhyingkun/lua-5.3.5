#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void usage(char* cmdName) {
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

static const char* const ActualTagNames[9] = {
    "LUA_TNIL",
    "LUA_TBOOLEAN",
    "LUA_TLIGHTUSERDATA",
    "LUA_TNUMBER",
    "LUA_TSTRING",
    "LUA_TTABLE",
    "LUA_TFUNCTION",
    "LUA_TUSERDATA",
    "LUA_TTHREAD",
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
static const int AllTypeNumber[13] = {
    0, // nil
    1, // boolean
    2, // light userdata
    3, // float number
    19, // integer number
    68, // short string
    84, // long string
    69, // table
    70, // lua closure
    22, // light c function
    102, // c closure
    71, // userdata
    72, // thread
};

bool is_type_num_valid(int type) {
  int length = sizeof(AllTypeNumber) / sizeof(int);
  for (int i = 0; i < length; i++) {
    if (type == AllTypeNumber[i]) {
      return true;
    }
  }
  return false;
}

void display_type(int tagType) {
  int actualTag = tagType & 0x0f;
  int variantBits = (tagType >> 4) & 0x03;
  int collectable = (tagType >> 6) & 0x01;
  printf("%3d => ActualTag: %s", tagType, ActualTagNames[actualTag]);
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

void display_all_type() {
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
  if (is_type_num_valid(tagType) == false) {
    fprintf(stderr, "Tag type %d is invalid!\n", tagType);
    return 0;
  }
  display_type(tagType);
  return 0;
}
