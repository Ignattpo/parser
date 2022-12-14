#include <dlfcn.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "func.h"

void resolve_variable(int socket, char* variable, long* res) {
  char buff[1024];

  dlerror();
  void* ptr = dlsym(NULL, variable);
  void* error = dlerror();
  if (!error) {
    sprintf(buff, "Symbol '%s' at %p\n", variable, ptr);
  } else {
    sprintf(buff, "Symbol '%s' not found\n", variable);
  }

  send(socket, buff, strlen(buff), 0);
  *res = ptr;
}

void resolve_address(int socket, long addr) {
  char buff[1024];
  void* ptr = (void*)addr;
  Dl_info info;
  int res = dladdr(ptr, &info);
  if (res != 0) {
    sprintf(buff, "Address '%lx' located at %s within the program %s\n", addr,
            info.dli_fname, info.dli_sname);
  } else {
    sprintf(buff, "Address '%lx' not found\n", addr);
  }

  send(socket, buff, strlen(buff), 0);
}

enum type_ptr_t { UNKNOWN = 0, U8, U16, U32 };

static enum type_ptr_t get_type_ptr(char* argv) {
  if (!strcmp(argv, "u8")) {
    return U8;
  }
  if (!strcmp(argv, "u16")) {
    return U16;
  }
  if (!strcmp(argv, "u32")) {
    return U32;
  }

  return UNKNOWN;
}

void read_variable(int socket, char* type, char* variable, long* res) {
  char buff[1024];
  dlerror();
  void* ptr = dlsym(NULL, variable);
  void* error = dlerror();
  if (error) {
    sprintf(buff, "Symbol '%s' not found\n", variable);
    send(socket, buff, strlen(buff), 0);
    return;
  }

  enum type_ptr_t type_ptr = get_type_ptr(type);
  switch (type_ptr) {
    case UNKNOWN:
      sprintf(buff, "Type '%s' not found\n", type);
      break;
    case U8:
      *res = *(uint8_t*)ptr;
      sprintf(buff, "Address '%s' = 0x%x\n", variable, *(uint8_t*)ptr);
      break;
    case U16:
      *res = *(uint16_t*)ptr;
      sprintf(buff, "Address '%s' = 0x%x\n", variable, *(uint16_t*)ptr);
      break;
    case U32:
      *res = *(uint32_t*)ptr;
      sprintf(buff, "Address '%s' = 0x%x\n", variable, *(uint32_t*)ptr);
      break;
  }

  send(socket, buff, strlen(buff), 0);
}

void read_address(int socket, char* type, long addr, long* res) {
  char buff[1024];
  void* ptr = (void*)addr;
  if (!ptr) {
    sprintf(buff, "Address '%lx' is NULL\n", addr);
    send(socket, buff, strlen(buff), 0);
    return;
  }

  enum type_ptr_t type_ptr = get_type_ptr(type);
  switch (type_ptr) {
    case UNKNOWN:
      sprintf(buff, "Type '%s' not found\n", type);
      break;
    case U8:
       *res = *(uint8_t*)ptr;
      sprintf(buff, "Address '%lx' = 0x%x\n", addr, *(uint8_t*)ptr);
      break;
    case U16:
       *res = *(uint16_t*)ptr;
      sprintf(buff, "Address '%lx' = 0x%x\n", addr, *(uint16_t*)ptr);
      break;
    case U32:
       *res = *(uint32_t*)ptr;
      sprintf(buff, "Address '%lx' = 0x%x\n", addr, *(uint32_t*)ptr);
      break;
  }

  send(socket, buff, strlen(buff), 0);
}

void write_variable(int socket, char* type, char* variable, long data) {
  char buff[1024];
  dlerror();
  void* ptr = dlsym(NULL, variable);
  void* error = dlerror();
  if (error) {
    sprintf(buff, "Symbol '%s' not found\n", variable);
    send(socket, buff, strlen(buff), 0);
    return;
  }

  enum type_ptr_t type_ptr = get_type_ptr(type);
  void* ptr_data = &data;

  switch (type_ptr) {
    case UNKNOWN:
      sprintf(buff, "Type '%s' not found\n", type);
      break;
    case U8:
      *(uint8_t*)ptr = *(uint8_t*)ptr_data;
      sprintf(buff, "Writed address '%s' = 0x%x\n", variable, *(uint8_t*)ptr);
      break;
    case U16:
      *(uint16_t*)ptr = *(uint16_t*)ptr_data;
      sprintf(buff, "Writed address '%s' = 0x%x\n", variable, *(uint16_t*)ptr);
      break;
    case U32:
      *(uint32_t*)ptr = *(uint32_t*)ptr_data;
      sprintf(buff, "Writed address '%s' = 0x%x\n", variable, *(uint32_t*)ptr);
      break;
  }

  send(socket, buff, strlen(buff), 0);
}

void write_address(int socket, char* type, long addr, long data) {
  char buff[1024];
  void* ptr = (void*)addr;
  if (!ptr) {
    sprintf(buff, "Address '%lx' is NULL\n", addr);
    send(socket, buff, strlen(buff), 0);
    return;
  }

  enum type_ptr_t type_ptr = get_type_ptr(type);
  void* ptr_data = &data;

  switch (type_ptr) {
    case UNKNOWN:
      sprintf(buff, "Type '%s' not found\n", type);
      break;
    case U8:
      *(uint8_t*)ptr = *(uint8_t*)ptr_data;
      sprintf(buff, "Writed address '%lx' = 0x%x\n", addr, *(uint8_t*)ptr);
      break;
    case U16:
      *(uint16_t*)ptr = *(uint16_t*)ptr_data;
      sprintf(buff, "Writed address '%lx' = 0x%x\n", addr, *(uint16_t*)ptr);
      break;
    case U32:
      *(uint32_t*)ptr = *(uint32_t*)ptr_data;
      sprintf(buff, "Writed address '%lx' = 0x%x\n", addr, *(uint32_t*)ptr);
      break;
  }

  send(socket, buff, strlen(buff), 0);
}

static void memory_dump(int socket, void* ptr, long byte_count) {
  char buff[1024];
  if (byte_count < 0) {
    char err[255];
    sprintf(err, "mem_dump command incorrect number of bytes(%ld)\n",
            byte_count);
    send(socket, err, strlen(err), 0);
    return;
  }
  //  ???????????? buff 1024 ???? ???????????? ?????????????? ?????????? ???????????? 5 ???????? ?? ???? ???????????? 8
  //  ?????????????????????? ?????? 3 ?????????? ???? ???? ???????????????? ???????????? 1024/5-((1024/5)/8)*3~=100
  if (byte_count > 100) {
    char err[255];
    sprintf(err,
            "the maximum number of received bytes at a time should not exceed "
            "100(%ld>100)\n",
            byte_count);
    send(socket, err, strlen(err), 0);
    return;
  }
  int j = 0;
  for (int i = 0; i < byte_count; i++) {
    sprintf(&buff[j], "0x%02x ", *(uint8_t*)ptr);
    j += 5;
    if (((i + 1) % 8) == 0) {
      ++j;
      sprintf(&buff[j], "\n");
      j += 2;
    }
    ptr++;
  }
  ++j;
  sprintf(&buff[j], "\n");
  send(socket, buff, j, 0);
}

void memory_dump_address(int socket, long addr, long byte_count) {
  void* ptr = (void*)addr;
  if (!ptr) {
    char err[255];
    sprintf(err, "Address '%lx' is NULL\n", addr);
    send(socket, err, strlen(err), 0);
    return;
  }

  memory_dump(socket, ptr, byte_count);
}

void memory_dump_variable(int socket, char* variable, long byte_count) {
  char buff[1024];
  dlerror();
  void* ptr = dlsym(NULL, variable);
  void* error = dlerror();
  if (error) {
    sprintf(buff, "Symbol '%s' not found\n", variable);
    send(socket, buff, strlen(buff), 0);
    return;
  }

  memory_dump(socket, ptr, byte_count);
}

static long get_number(char* argv) {
  int base = 10;
  if ((argv[0] == '0') && (argv[1] == 'x')) {
    base = 16;
  }

  return strtoul(argv, NULL, base);
}

static int memory_write(void* ptr, char* dump) {
  char* cnum;
  cnum = strtok(dump, " ");
  int count = 0;
  while (cnum != NULL) {
    void* ptr_data = NULL;
    long number = get_number(cnum);
    ptr_data = &number;
    *(uint8_t*)ptr = *(uint8_t*)ptr_data;
    ++ptr;
    ++count;
    cnum = strtok(NULL, " ");
  }
  return count;
}

void memory_write_address(int socket, long addr, char* dump) {
  void* ptr = (void*)addr;
  if (!ptr) {
    char err[255];
    sprintf(err, "Address '%lx' is NULL\n", addr);
    send(socket, err, strlen(err), 0);
    return;
  }

  int count = memory_write(ptr, dump);
  char buff[255];
  sprintf(buff, "Writed %d bytes to address %lx\n", count, addr);
  send(socket, buff, strlen(buff), 0);
}

void memory_write_variable(int socket, char* variable, char* dump) {
  dlerror();
  void* ptr = dlsym(NULL, variable);
  void* error = dlerror();
  if (error) {
    char err[255];
    sprintf(err, "Symbol '%s' not found\n", variable);
    send(socket, err, strlen(err), 0);
    return;
  }

  int count = memory_write(ptr, dump);
  char buff[255];
  sprintf(buff, "Writed %d bytes to address %s\n", count, variable);
  send(socket, buff, strlen(buff), 0);
}

static void memory_dump_to_file(int socket,
                                char* file_name,
                                void* ptr,
                                long byte_count) {
  if (byte_count < 0) {
    char err[255];
    sprintf(err, "mem_dump command incorrect number of bytes(%ld)\n",
            byte_count);
    send(socket, err, strlen(err), 0);
    return;
  }
  //  ???????????? buff 1024 ???? ???????????? ?????????????? ?????????? ???????????? 5 ???????? ?? ???? ???????????? 8
  //  ?????????????????????? ?????? 3 ?????????? ???? ???? ???????????????? ???????????? 1024/5-((1024/5)/8)*3~=100
  if (byte_count > 100) {
    char err[255];
    sprintf(err,
            "the maximum number of received bytes at a time should not exceed "
            "100(%ld>100)\n",
            byte_count);
    send(socket, err, strlen(err), 0);
    return;
  }
  FILE* fp = NULL;
  if ((fp = fopen(file_name, "w")) == NULL) {
    char err[255];
    sprintf(err, "mem_dump_to_file Failed to open file %s\n", file_name);
    send(socket, err, strlen(err), 0);
    return;
  }

  int j = 0;
  for (int i = 0; i < byte_count; i++) {
    fprintf(fp, "0x%02x ", *(uint8_t*)ptr);
    j += 5;
    if (((i + 1) % 8) == 0) {
      ++j;
      fprintf(fp, "\n");
      j += 2;
    }
    ptr++;
  }
  fclose(fp);

  char buff[255];
  sprintf(buff, "memory dump written to file %s \n", file_name);
  send(socket, buff, strlen(buff), 0);
}

void memory_dump_to_file_address(int socket,
                                 char* file_name,
                                 long addr,
                                 long byte_count) {
  void* ptr = (void*)addr;
  if (!ptr) {
    char err[255];
    sprintf(err, "Address '%lx' is NULL\n", addr);
    send(socket, err, strlen(err), 0);
    return;
  }

  memory_dump_to_file(socket, file_name, ptr, byte_count);
}

void memory_dump_to_file_variable(int socket,
                                  char* file_name,
                                  char* variable,
                                  long byte_count) {
  dlerror();
  void* ptr = dlsym(NULL, variable);
  void* error = dlerror();
  if (error) {
    char err[255];
    sprintf(err, "Symbol '%s' not found\n", variable);
    send(socket, err, strlen(err), 0);
    return;
  }

  memory_dump_to_file(socket, file_name, ptr, byte_count);
}
