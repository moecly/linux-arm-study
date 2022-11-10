#include <fcntl.h>
#include <sys/poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define True 0
#define False -1

int main(int argc, char **argv) {
  int fd;
  int val;
  struct pollfd fds[1];
  int timeout_ms = 5000;
  int ret;

  if (argc != 2) {
    printf("input err\n");
    return False;
  }

  fd = open(argv[1], O_RDWR);
  if (fd < 0) {
    printf("open %s err\n", argv[1]);
    return False;
  }

  fds[0].fd = fd;
  fds[0].events = POLLIN;

  while (1) {
    ret = poll(fds, 1, timeout_ms);
    if ((ret == 1) && (fds[0].revents & POLLIN)) {
      read(fd, &val, 4);
      printf("button get = 0x%x\n", val);
    } else {
      printf("timeout\n");
    }
  }

  close(fd);

  return 0;
}