#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define True 0
#define False -1

int main(int argc, char **argv) {
  int fd;
  int buf;

  if (argc != 2) {
    printf("input err\n");
    return False;
  }

  fd = open(argv[1], O_RDWR);
  if (fd < 0) {
    printf("open %s err\n", argv[1]);
    return False;
  }

  while (1) {
    printf("test\n");
    read(fd, &buf, 4);
    printf("buf = 0x%x\n", buf);
  }

  close(fd);

  return 0;
}