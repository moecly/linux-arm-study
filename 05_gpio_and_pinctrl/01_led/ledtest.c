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
  int err;
  char status = 1;
  if (argc != 3) {
    printf("input err\n");
    goto out;
  }

  fd = open(argv[1], O_RDWR);
  if (fd < 0) {
    printf("can't open %s\n", argv[1]);
    goto out;
  }

  if (strcmp(argv[2], "on")) {
    status = 0;
  }

  err = write(fd, &status, 1);
  if (err != 1) {
    printf("write err\n");
    goto out;
  }

  return True;

out:
  return False;
}