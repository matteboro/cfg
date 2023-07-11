#include "../prsr/prsr.h"
#include "strct_graph.h"
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
  return data;
}

int main() {

  char *code = file_to_cstring("/home/matteo/github/cfg/chckr/code.b");

  ASTNode *ast = prsr_parse(code);

  ProgramNodeData *program_data = (ProgramNodeData *) ast->data;
  NodeListData *structs_list_data = (NodeListData *) program_data->struct_declarations->data;
  StructGraph* struct_graph = strct_graph_maker(structs_list_data->nodes);

  strct_graph_dump_dot(struct_graph, stdout);
  strct_graph_dealloc(struct_graph);

  ast_print_node(ast, stdout); fprintf(stdout, "\n");
  ast_dealloc_node(ast);

  munmap(code, 0);

  return 0;
}