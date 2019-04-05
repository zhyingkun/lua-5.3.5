#include <stdio.h>
#include <stdlib.h>

void usage(char* cmdName) {
  printf("\tusage: %s <TagTypeNumber>\n\tTagTypeNumber: Integer Number\n",
         cmdName);
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
    "LUA_TNIL",      "LUA_TBOOLEAN",  "LUA_TLIGHTUSERDATA",
    "LUA_TNUMBER",   "LUA_TSTRING",   "LUA_TTABLE",
    "LUA_TFUNCTION", "LUA_TUSERDATA", "LUA_TTHREAD"};
static const char* const VariantTagFunctions[3] = {
    "Lua Closure", "Light C Function", "C Closure"};
static const char* const VariantTagStrings[2] = {"Short Strings",
                                                 "Long Strings"};
static const char* const VariantTagNumbers[2] = {"Float Numbers",
                                                 "Integer Numbers"};
static const char* const IsCollectable[2] = {"Not Collectable", "Collectable"};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    usage(argv[0]);
    return -1;
  }
  int tagType = atoi(argv[1]);
  int actualTag = tagType & 0x0f;
  int variantBits = (tagType >> 4) & 0x03;
  int collectable = (tagType >> 6) & 0x01;
  printf("actual tag: %s\n", ActualTagNames[actualTag]);
  const char* variantName = NULL;
  switch (actualTag) {
    case LUA_TFUNCTION:
      variantName = VariantTagFunctions[variantBits];
      break;
    case LUA_TSTRING:
      variantName = VariantTagStrings[variantBits];
      break;
    case LUA_TNUMBER:
      variantName = VariantTagNumbers[variantBits];
      break;
    default:
      variantName = "No Variant Tag Type";
      break;
  }
  printf("variant tag: %s\n", variantName);
  printf("is collectable: %s\n", IsCollectable[collectable]);
  return 0;
}
