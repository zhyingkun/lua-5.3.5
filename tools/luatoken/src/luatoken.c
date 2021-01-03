#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_RESERVED 257

// clang-format off
enum RESERVED {
  /* terminal symbols denoted by reserved words */
  TK_AND = FIRST_RESERVED, TK_BREAK,
  TK_DO, TK_ELSE, TK_ELSEIF, TK_END, TK_FALSE, TK_FOR, TK_FUNCTION,
  TK_GOTO, TK_IF, TK_IN, TK_LOCAL, TK_NIL, TK_NOT, TK_OR, TK_REPEAT,
  TK_RETURN, TK_THEN, TK_TRUE, TK_UNTIL, TK_WHILE,
  /* other terminal symbols */
  TK_IDIV, TK_CONCAT, TK_DOTS, TK_EQ, TK_GE, TK_LE, TK_NE,
  TK_SHL, TK_SHR,
  TK_DBCOLON, TK_EOS,
  TK_FLT, TK_INT, TK_NAME, TK_STRING
};
// clang-format on

// clang-format off
static const char* const luaX_tokennames[] = {
  "TK_AND", "TK_BREAK",
  "TK_DO", "TK_ELSE", "TK_ELSEIF", "TK_END", "TK_FALSE", "TK_FOR", "TK_FUNCTION",
  "TK_GOTO", "TK_IF", "TK_IN", "TK_LOCAL", "TK_NIL", "TK_NOT", "TK_OR", "TK_REPEAT",
  "TK_RETURN", "TK_THEN", "TK_TRUE", "TK_UNTIL", "TK_WHILE",
  /* other terminal symbols */
  "TK_IDIV", "TK_CONCAT", "TK_DOTS", "TK_EQ", "TK_GE", "TK_LE", "TK_NE",
  "TK_SHL", "TK_SHR",
  "TK_DBCOLON", "TK_EOS",
  "TK_FLT", "TK_INT", "TK_NAME", "TK_STRING"
};
// clang-format on

// clang-format off
static const char* const luaX_tokens[] = {
    "and", "break", "do", "else", "elseif",
    "end", "false", "for", "function", "goto", "if",
    "in", "local", "nil", "not", "or", "repeat",
    "return", "then", "true", "until", "while",
    "//", "..", "...", "==", ">=", "<=", "~=",
    "<<", ">>", "::", "<eof>",
    "<number>", "<integer>", "<name>", "<string>"
};

static const char luaX_othertokens[] = {
    '=', '<', '>', '/', '~', ':', '.', '+', '-', '*', '(', ')',
    '[', ']', '{', '}', ',', ';', '%', '^', '#', '&', '|', '\0',
};
// clang-format on

static void usage(char* cmdName) {
  printf("\tusage: %s <TokenNumber>\n\tTokenNumber: Integer Number\n", cmdName);
}

#define is_token_valid(token) ((token >= TK_AND && token <= TK_STRING) || strchr(luaX_othertokens, token))

static void display_token(int token) {
  if (token >= TK_AND && token <= TK_STRING) {
    int index = token - FIRST_RESERVED;
    printf("\t%d:\t%s\t\"%s\"\n", token, luaX_tokennames[index], luaX_tokens[index]);
  } else {
    printf("\t%d:\t'%c'\n", token, token);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    usage(argv[0]);
    return 0;
  }
  int token = atoi(argv[1]);
  if (is_token_valid(token) == 0) {
    fprintf(stderr, "Token %d is invalid!\n", token);
    return 0;
  }
  display_token(token);
  return 0;
}
