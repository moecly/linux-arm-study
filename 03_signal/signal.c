#include "unistd.h"
#include <signal.h>
#include <stdio.h>

void my_sig_fun(int signo) { printf("get a sianal %d\n", signo); }

int main(int argc, char **argv) {
  int i = 0;
  signal(SIGIO, my_sig_fun);

  while (1) {
    printf("hello world %d\n", i++);
    sleep(2);
  }

  return 0;
}