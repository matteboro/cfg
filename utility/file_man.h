#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

char *file_to_cstring(const char *filename) {
  int fd = open(filename, O_RDONLY);
  // fprintf(stdout, "fd: %d\n", fd);
  off_t len = lseek(fd, 0, SEEK_END);
  // fprintf(stdout, "len: %ld\n", len);
  void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  return (char *) data;
}
