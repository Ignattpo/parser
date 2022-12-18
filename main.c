#include <stdbool.h>
#include <stdio.h>

#include "telnet_dbg.h"

int main(int argc, char** argv) {
  struct telnet_dbg_t* dbg = telnet_dbg_init("127.0.0.1", 3025);
  telnet_dbg_run(dbg);
  while (1) {
    sleep(1);
  }

  telnet_dbg_stop(dbg);

  telnet_dbg_free(dbg);

  return 0;
}
