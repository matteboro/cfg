#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "string.h"
#include "assert.h"

void red(FILE *file) {
  fprintf(file, "\033[1;31m"); 
} 

void reset(FILE *file) {
  fprintf(file, "\033[0m");
} 

typedef struct {
  char *name;
  const char *data;
  size_t length;
} File;

typedef struct {
  size_t start_col, end_col;
  size_t start_line, end_line;
  size_t data_length;
} FilePosition;

typedef struct {
  File *file;
  FilePosition position;
} FileInfo;

char *file_to_cstring(const char *filename) {
  int fd = open(filename, O_RDONLY);
  // fprintf(stdout, "fd: %d\n", fd);
  off_t len = lseek(fd, 0, SEEK_END);
  // fprintf(stdout, "len: %ld\n", len);
  void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  return (char *) data;
}

File *file_open(const char* file_path) {
  File *file = (File *) malloc(sizeof(File));
  file->data = file_to_cstring(file_path);
  file->length = strlen(file->data);
  file->name = (char *) malloc(strlen(file_path)+1);
  strcpy(file->name, file_path);
  return file;
}

void file_dealloc(File *file) {
  free(file->name);
  munmap((void *)file->data, 0);
  free(file);
}

FileInfo file_info_create_null() {
  FileInfo f_info = {
    .file = NULL,
    .position = {
      .data_length = 0,
      .end_col = 0,
      .end_line = 0,
      .start_col = 0,
      .start_line = 0
    }
  };
  return f_info;
}

int file_info_is_null(FileInfo f) {
  return 
    f.file == NULL && 
      f.position.data_length == 0 && 
      f.position.end_col == 0 &&
      f.position.start_col == 0 && 
      f.position.end_line == 0 &&
      f.position.start_line == 0;
}

FileInfo file_info_merge(FileInfo f1, FileInfo f2) {
  // fprintf(stdout, "fi.name: %s, f2.name: %s\n", f1.file->name, f1.file->name);
  // fprintf(stdout, "fi.file_ptr: %lu, f2.file_ptr: %lu\n", f1.file, f2.file);
  assert(!file_info_is_null(f1));
  assert(!file_info_is_null(f2));
  assert(f1.file == f2.file);

  size_t start_col, end_col, start_line, end_line;

  if (f1.position.start_line  == f2.position.start_line) {
    start_line = f1.position.start_line;
    start_col = f1.position.start_col <= f2.position.start_col ? f1.position.start_col : f2.position.start_col;
  } 
  else if (f1.position.start_line  < f2.position.start_line) {
    start_line = f1.position.start_line;
    start_col = f1.position.start_col;
  } 
  else {
    start_line = f2.position.start_line;
    start_col = f2.position.start_col;
  }

  if (f1.position.end_line  == f2.position.end_line) {
    end_line = f1.position.end_line;
    end_col = f1.position.end_col >= f2.position.end_col ? f1.position.end_col : f2.position.end_col;
  } 
  else if (f1.position.end_line > f2.position.end_line) {
    end_line = f1.position.end_line;
    end_col = f1.position.end_col;
  } 
  else {
    end_line = f2.position.end_line;
    end_col = f2.position.end_col;
  }

  FileInfo f_res = {
    .file = f1.file,
    .position = {
      .data_length = f1.position.data_length + f2.position.data_length,
      .start_line =  start_line,
      .end_line = end_line,
      .start_col =  start_col,
      .end_col = end_col,
    }
  };

  return f_res;
}

void file_info_print_highlighted(FileInfo file_info, FILE* out_file) {
  size_t curr_line = 0;
  size_t curr_col = 0;
  File *file = file_info.file;
  for (size_t i = 0; i < file->length; ++i) {
    ++curr_col;
    if (file->data[i] == '\n') {
      ++curr_line;
      curr_col = 0;
    }
    if (curr_line >= file_info.position.start_line) {
      if (curr_col >= file_info.position.start_col) {
        red(out_file);
      } 
    }
    if (curr_line >= file_info.position.end_line) {
      if (curr_col >= file_info.position.end_col) {
        reset(out_file);
      } 
    }
    fprintf(out_file, "%c", file->data[i]);
  }
}