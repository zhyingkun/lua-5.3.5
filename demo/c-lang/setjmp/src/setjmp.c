#include <stdio.h>
#include <setjmp.h>

static jmp_buf env;

double divide(double to, double by) {
  if (by == 0)
    longjmp(env, 1);
  return to / by;
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  printf("Start...\n");
  // wrong usage
  // int ret = setjmp(env);
  // if (ret == 0)
  // {
  //     printf("ret:%d\n", ret);
  //     divide(2, 0);
  // }
  // else
  // {
  //     printf("Cannot / 0  ret:%d\n", ret);
  // }
  // right usage
  switch (setjmp(env)) {
    case 0:
      printf("setjmp return 0\n");
      divide(2, 0);
      break;
    case 1:
      printf("setjmp return 1\n");
      printf("Cannot divide 0\n");
      break;
    default:
      printf("setjmp come to default\n");
      break;
  }
  printf("End\n");
  return 0;
}
