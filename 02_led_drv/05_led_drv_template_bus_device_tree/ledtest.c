#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void delay_ms(unsigned int t) {
  unsigned int i = 0;
  for (; i < t; i++) {
  }
}

void delay(unsigned int t) {
  unsigned int i = 0;
  for (; i < t; i++)
    delay_ms(100);
}

/*
 * ./ledtest /dev/100ask_led0 on
 * ./ledtest /dev/100ask_led0 off
 */
int main(int argc, char **argv) {
  int fd;
  char status;

  /* 1. 判断参数 */
  if (argc != 3) {
    printf("Usage: %s <dev> <on | off>\n", argv[0]);
    return -1;
  }

  /* 2. 打开文件 */
  fd = open(argv[1], O_RDWR);
  if (fd == -1) {
    printf("can not open file %s\n", argv[1]);
    return -1;
  }

  /* 3. 写文件 */
  if (0 == strcmp(argv[2], "on")) {
    status = 1;
  } else if (0 == strcmp(argv[2], "off")) {
    status = 0;
  } else {
    status = 2;
    while (1) {
      write(fd, &status, 1);
      delay(50000);
    }
  }

  write(fd, &status, 1);
  close(fd);

  return 0;
}
