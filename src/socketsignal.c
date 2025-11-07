#include "socketsignal.h"

// This function handles termination signals by closing the global file descriptor.
// Since it is not exposed outside this file, it is declared as static.
static void signalHandler(){
  printf("\nTermination signal received. Closing socket\n");

  if(fd != -1){
    close(fd);
    fd = -1;
  }

  _exit(0);
}

void installSignalHandler(){
  struct sigaction sa;
  sa.sa_handler = signalHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
}

void resetSignalHandler(){
  struct sigaction sa;
  sa.sa_handler = SIG_DFL;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
}