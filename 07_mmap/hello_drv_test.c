#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * ./hello_drv_test -w abc
 * ./hello_drv_test -r
 */
int main(int argc, char **argv) {
  int fd;
  char *buf;
  char str[1024];
  int len;

  /* 1. 判断参数 */
  if (argc < 2) {
    printf("Usage: %s -w <string>\n", argv[0]);
    printf("       %s -r\n", argv[0]);
    return -1;
  }

  /* 2. 打开文件 */
  fd = open("/dev/hello", O_RDWR);
  if (fd == -1) {
    printf("can not open file /dev/hello\n");
    return -1;
  }

  buf = mmap(NULL, 1024 * 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (buf == MAP_FAILED) {
    close(fd);
    return -1;
  }

  strcpy(buf, "new");
  printf("mmap address: 0x%x\n", buf);
  printf("buf origin data = %s\n", buf);
  read(fd, str, 1024);

  if (strcmp(buf, str) == 0) {
    printf("compare succ\n");
  } else {
    printf("compare err\n");
  }
  printf("str = %s\n", str);

  while (1) {
    sleep(10);
  }

  /* 3. 写文件或读文件 */
  close(fd);

  return 0;
}
