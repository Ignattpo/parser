#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "telnet_dbg.h"

#define ADDRESS "127.0.0.1"
#define PORT 3015

volatile unsigned char terminated = 0;

// void sigint_cb(int sig) {
//  if (!terminated) {
//    terminated = 1;
//  }
//}

// static void signals_init(void) {
//  /* Обработчик сигнала SIGINT */
//  struct sigaction sa_hup;
//  memset(&sa_hup, 0, sizeof(sa_hup));
//  sa_hup.sa_handler = sigint_cb;
//  sa_hup.sa_flags = SA_RESTART;
//  sigaction(SIGINT, &sa_hup, 0);
//  sigaction(SIGTERM, &sa_hup, 0);
//}

int main(int argc, char** argv) {
  //  signals_init();

  struct telnet_dbg_t* dbg = telnet_dbg_init(ADDRESS, PORT);
  telnet_dbg_run(dbg);

  while (!terminated) {
    sleep(1);
  }

  telnet_dbg_stop(dbg);
  telnet_dbg_free(dbg);

  return 0;
}
