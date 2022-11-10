#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define True 0
#define False -1

int fd;

void sig_func(int sig) {
  int val;
  read(fd, &val, 4);
  printf("get button from sig = 0x%x\n", val);
}

int main(int argc, char **argv) {
  int val;
  struct pollfd fds[1];
  int timeout_ms = 5000;
  int flags;
  int ret;

  if (argc != 2) {
    printf("input err\n");
    return False;
  }

  signal(SIGIO, sig_func);

  fd = open(argv[1], O_RDWR);
  if (fd < 0) {
    printf("open %s err\n", argv[1]);
    return False;
  }

  fcntl(fd, F_SETOWN, getpid());
  flags = fcntl(fd, F_GETFL);
  fcntl(fd, F_SETFL, flags | FASYNC);

  while (1) {
  }

  close(fd);

  return 0;
}