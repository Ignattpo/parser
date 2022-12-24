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

// static const parser_command cmds[] = {
//    {"c", -1, &cmd_call, "Call function by symbol/address", NULL},
//    {"hw", 2, &cmd_writehex, "Write hex string to address", NULL},
//    {"hd", 2, &cmd_dump, "HexDump memory to console.", NULL},
//    {"m2f", 3, &mem2file_cmd, "Dump memory to file", NULL},
//    {"ww", 2, &memwrite_cmd, "Write WORD", NULL},
//    {"exit", 0, &cmd_exit, "Close shell", NULL},
//    {NULL, 0, NULL, NULL}};

//  static void cmd_call(int argc, char** argp, void* data, char quoted[]) {
//    printf(">>>>>> CMD_CALL!!\n");
//    int fd = *(int*)data;
//    char sym[512];
//    char tmp[0xFFF];
//    void* symptr;
//    void *a1, *a2, *a3, *a4, *a5;
//    void* fptr;
//    int (*fptr1)(void*);
//    int (*fptr2)(void*, void*);
//    int (*fptr3)(void*, void*, void*);
//    int (*fptr4)(void*, void*, void*, void*);
//    int (*fptr5)(void*, void*, void*, void*, void*);
//    unsigned retcode = 0;

//    // printf("ARGC=%i %s %s %s\n", argc, argp[0], argp[1], argp[2]);

//    // printf(">>>> CMD______CALL %i %i %i %i %i  \n", (int)quoted[0],
//    // (int)quoted[1], (int)quoted[2], (int)quoted[3], (int)quoted[4]);

//    if (argc > 2)
//      a1 = arg2arg(argp[2], ALLOW_ALL, quoted[2]);
//    if (argc > 3)
//      a2 = arg2arg(argp[3], ALLOW_ALL, quoted[3]);
//    if (argc > 4)
//      a3 = arg2arg(argp[4], ALLOW_ALL, quoted[4]);
//    if (argc > 5)
//      a4 = arg2arg(argp[5], ALLOW_ALL, quoted[5]);
//    if (argc > 6)
//      a5 = arg2arg(argp[6], ALLOW_ALL, quoted[6]);

//    strlen("Hello");
//    symptr = dlsym(NULL, argp[1]);

//    sprintf(tmp, ">>> SYM=%s ptr=%p\n", argp[1], symptr);
//    printf(">>> %s\n", tmp);

//    send(fd, tmp, strlen(tmp), 0);
//    fptr = symptr;
//    if (fptr) {
//      if (argc == 3) {
//        fptr1 = fptr;
//        retcode = fptr1(a1);
//      }

//      if (argc == 4) {
//        fptr2 = fptr;
//        retcode = fptr2(a1, a2);
//      }

//      if (argc == 5) {
//        fptr3 = fptr;
//        retcode = fptr3(a1, a2, a3);
//      }

//      if (argc == 6) {
//        fptr4 = fptr;
//        retcode = fptr4(a1, a2, a3, a4);
//      }

//      if (argc == 7) {
//        fptr5 = fptr;
//        retcode = fptr5(a1, a2, a3, a4, a5);
//      }

//      // int ret = (*fptr)(111);
//      // sprintf(tmp, "Ret code: %i (0x%08X)\n", retcode, retcode);
//      printf2sock(fd, ">>> Ret code: %i (0x%08X)\n", retcode, retcode);
//      // printf(tmp);
//      // send(fd, tmp, strlen(tmp), 0);
//    }
//  }

//  void cmd_resolve(int argc, char** argp, void* data) {
//    void* ptr;
//    int fd = *(int*)data;

//    Dl_info info;
//    int rc;
//    void* symptr;

//    if (string_to_num(argp[1], &ptr) != -1) {
//      rc = dladdr(ptr, &info);
//      if (!rc) {
//        printf2sock(fd, "Problem retrieving program information for %p: %s\n",
//                    ptr, dlerror());
//        return;
//      }
//      printf2sock(fd,
//                  "Address '%p' located in function %s within the program
//                  %s\n", info.dli_saddr, info.dli_fname, info.dli_sname);
//    } else {
//      symptr = dlsym(NULL, argp[1]);
//      if (!symptr) {
//        printf2sock(fd, "Problem retrieving information for '%s':  %s\n",
//                    argp[1], dlerror());
//        return;
//      }
//      printf2sock(fd, "Symbol '%s' located in address %p\n", argp[1], symptr);
//    }
//  }

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

//  static void memread_cmd(int argc, char** argp, void* ddata) {
//    int fd = *(int*)ddata;
//    Dl_info info;
//    int rc;
//    void* ptr;
//    u32_t value;
//    u32_t old_value = 0;

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
//    value = *(unsigned*)ptr;

//    printf2sock(fd, ">>> Value 0x%08X (%u) at %p \n", value, value, ptr);
//  }

//  static void cmd_exit(int argc, char** argp, void* data) {
//    int fd = *(int*)data;
//    send(fd, "Bye!\n\n", 6, 0);
//    close(fd);
//  }
