#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ERR -1
#define SUCC 0

void my_delay_ms(unsigned int t) {
  unsigned int i = 0;
  for (; i < t; i++) {
  }
}

void delay(unsigned int t) {
  unsigned int i = 0;
  for (; i < t; i++)
    my_delay_ms(1000);
}

int main(int argc, char **argv) {
  int err;
  int fd;
  char res;
  if (argc != 2) {
    printf("err input\n");
    return ERR;
  }

  /* 打开button */
  fd = open(argv[1], O_RDWR);
  if (fd < 0) {
    printf("open %s failed\n", argv[1]);
  }

  printf("test");
  while (1) {
    while (read(fd, &res, 1) != 0) {
    }

    delay(50);
  }

  return SUCC;
}