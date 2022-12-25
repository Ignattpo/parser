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

void resolve_variable(int socket, char* variable) {
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

void read_variable(int socket, char* type, char* variable) {
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
      sprintf(buff, "Address '%s' = 0x%x\n", variable, *(uint8_t*)ptr);
      break;
    case U16:
      sprintf(buff, "Address '%s' = 0x%x\n", variable, *(uint16_t*)ptr);
      break;
    case U32:
      sprintf(buff, "Address '%s' = 0x%x\n", variable, *(uint32_t*)ptr);
      break;
  }

  send(socket, buff, strlen(buff), 0);
}

void read_address(int socket, char* type, long addr) {
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
      sprintf(buff, "Address '%lx' = 0x%x\n", addr, *(uint8_t*)ptr);
      break;
    case U16:
      sprintf(buff, "Address '%lx' = 0x%x\n", addr, *(uint16_t*)ptr);
      break;
    case U32:
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
  //  Размер buff 1024 на запись каждого байта уходит 5 байт и на каждый 8
  //  добавляется еще 3 байта из за переноса строки 1024/5-((1024/5)/8)*3~=100
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

// static const parser_command cmds[] = {
//    {"hw", 2, &cmd_writehex, "Write hex string to address", NULL},
//    {"m2f", 3, &mem2file_cmd, "Dump memory to file", NULL},
//    {NULL, 0, NULL, NULL}};





//  static void cmd_writehex(int argc, char** argp, void* data) {
//    int fd = *(int*)data;
//    void* base;
//    printf2sock(fd, "A1=%s a2=%s\n", argp[1], argp[2]);
//    if (string_to_num(argp[1], &base) == -1) {
//      printf2sock(fd, ">>> Wrong arg: '%s'\n", argp[1]);
//      return;
//    }

//    char* bin = malloc(strlen(argp[2]));
//    memset(bin, 0, strlen(argp[2]));
//    int len = hex2bin(bin, argp[2]);
//    printf2sock(fd, ">>> Write %i bytes to addr %p:\n", len, base);
//    if (len > 0)
//      memcpy(base, bin, len);
//    return;
//  }

//  void cmd_dump(int argc, char** argp, void* ddata) {
//    int fd = *(int*)ddata;

//    // send(fd, "ECHO  dump!\n", 12, 0);

//    char ascii[17];
//    char tmp[512];
//    size_t i, j;
//    int size;
//    ascii[16] = '\0';
//    char* data;

//    // printf2sock(fd, "argc=%i argp=%p A0=%s A1=%s A2=%s\n", argc, argp,
//    // argp[0], argp[1], argp[2]);

//    if (string_to_num(argp[1], &data) == -1 ||
//        string_to_num(argp[2], &size) == -1) {
//      printf2sock(fd, "Wrong arguments\n");
//      return;
//    }

//    // printf2sock(fd, "HELLO printf2sock data=%p size=%i\n", data, size);

//    for (i = 0; i < size; ++i) {
//      printf2sock(fd, "%02X ", ((unsigned char*)data)[i]);
//      if (((unsigned char*)data)[i] >= ' ' &&
//          ((unsigned char*)data)[i] <= '~') {
//        ascii[i % 16] = ((unsigned char*)data)[i];
//      } else {
//        ascii[i % 16] = '.';
//      }
//      if ((i + 1) % 8 == 0 || i + 1 == size) {
//        printf2sock(fd, " ");
//        if ((i + 1) % 16 == 0) {
//          printf2sock(fd, "|  %s \n", ascii);
//        } else if (i + 1 == size) {
//          ascii[(i + 1) % 16] = '\0';
//          if ((i + 1) % 16 <= 8) {
//            printf2sock(fd, " ");
//          }
//          for (j = (i + 1) % 16; j < 16; ++j) {
//            printf2sock(fd, "   ");
//          }
//          printf2sock(fd, "|  %s \n", ascii);
//        }
//      }
//    }
//  }

//  static void mem2file_cmd(int argc, char** argp, void* ddata) {
//    int fd = *(int*)ddata;

//    char ascii[17];
//    char tmp[512];
//    size_t i, j;
//    int size;
//    ascii[16] = '\0';
//    char* data;

//    // printf2sock(fd, "argc=%i argp=%p A0=%s A1=%s A2=%s\n", argc, argp,
//    // argp[0], argp[1], argp[2]);

//    if (string_to_num(argp[1], &data) == -1 ||
//        string_to_num(argp[2], &size) == -1) {
//      printf2sock(fd, ">>> Wrong arguments\n");
//      return;
//    }

//    int res = mem2file(argp[3], data, size);
//    if (res == 0)
//      printf2sock(fd, ">>> Writed %u bytes to %s\n", size, argp[3]);
//    else
//      printf2sock(fd, ">>> Problem writing %u bytes to %s !!!!!\n", size,
//                  argp[3]);

//    return;
//  }

//  static void memwrite_cmd(int argc, char** argp, void* ddata) {
//    int fd = *(int*)ddata;
//    Dl_info info;
//    int rc;
//    void* ptr;
//    u32_t value;
//    u32_t old_value = 0;

//    value = arg2arg(argp[2], ALLOW_INT | ALLOW_SYM, 0);
//    /*
//    if (string_to_num(argp[2], &value) == -1)
//    {
//        printf2sock(fd, ">>> Wrong argument: '%s'\n", argp[2]);
//        return;
//    }
//    */

//    if (string_to_num(argp[1], &ptr) == -1) {
//      ptr = dlsym(NULL, argp[1]);
//      if (!ptr) {
//        printf2sock(fd, ">>> Problem retrieving information for '%s':  %s\n",
//                    argp[1], dlerror());
//        return;
//      }
//      printf2sock(fd, ">>> Symbol '%s' located in address %p\n", argp[1],
//      ptr);
//    }
//    old_value = *(unsigned*)ptr;
//    *(unsigned*)ptr = value;
//    printf2sock(fd, ">>> Value 0x%08X writed to addr %p (old value:
//    0x%08X)\n",
//                value, ptr, old_value);
//  }

