#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "socket.h"
#include "protocol.h"
#include "socketsignal.h"
#include "test.h"

static int fd = -1;

/**
 * Cleanup function to be called at program exit.
 * Closes the global socket file descriptor if it is open.
 */
void cleanup();

/**
 * Handles server responses by receiving a message and checking for errors.
 * Exits the program if an error is encountered.
 * @param msg Pointer to a Message structure to store the received message.
 * @param expect_flags Flags that are expected to be set in the response.
 */
void response(Message* msg, int expect_flags);

/**
 * Signal handler for termination signals.
 */
void signalHandler();

int main(int argc, char* argv[]){
  atexit(cleanup);
  installSignalHandler(signalHandler);

  Message msg;
  char buffer[BUFFER_SIZE];
  char name[BUFFER_SIZE];
  struct timeval timeout = {15, 0};

  if(argc == 2 && strcmp(argv[1], "--dev") == 0){
    printf("[Client]: Running in development mode. Connecting to localhost\n");
  }
  else{
    printf("[Client]: Connecting to server at %s:%d\n", SERVER_IP, SERVER_PORT);
  }

  fd = clientSocket(argv[1] && strcmp(argv[1], "--dev") == 0 ? "127.0.0.1" : SERVER_IP, SERVER_PORT);

  if(fd < 0){
    printf("[Client]: Unable to connect to the server\n");
    exit(EXIT_FAILURE);
  }

  setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

  printf("[Client]: Connected to the server\n\n");
  
  printf("Enter username: ");
  if(!fgets(name, BUFFER_SIZE, stdin)) exit(EXIT_FAILURE);

  sendMessage(fd, REQ_OK | NAME, name);
  response(&msg, RES_OK | NAME);

  name[strcspn(name, "\n")] = 0;

  printf("Enter password: ");
  if(!fgets(buffer, BUFFER_SIZE, stdin)) exit(EXIT_FAILURE);

  sendMessage(fd, REQ_OK | PASSWORD, buffer);
  response(&msg, RES_OK | PASSWORD);
  
  response(&msg, RES_OK | AUTH);
  printf("[Server]: %s\n", msg.data);

  while(1){
    printf("[%s]: ", name);
    if(!fgets(buffer, BUFFER_SIZE, stdin)) exit(EXIT_FAILURE);

    if(strcmp(buffer, "exit\n") == 0){
      sendMessage(fd, REQ_OK | CLOSE, "");
      exit(EXIT_SUCCESS);
    }

    sendMessage(fd, REQ_OK | DATA, buffer);
    response(&msg, RES_OK | DATA);

    printf("[Server]: %s\n", msg.data);
  }

  return 0;
}

void cleanup(){
  printf("\nCleaning up socket before exit...\n");

  if(fd >= 0){
    close(fd);
    fd = -1;
  }
}

void response(Message* msg, int expect_flags){
  int status = recvMessage(fd, msg);

  // Check if the message was received successfully
  if(status != MSG_OK){
    printf("[Server]: Unable to reach the server\n");
    exit(EXIT_FAILURE);
  }

  // Check for error flag in the message status
  if(hasFlag(msg->status, ERR_BIT)){
    printf("[Server]: %s\n", msg->data);
    exit(EXIT_FAILURE);
  }

  // Check for unexpected flags in the message status
  if(!hasFlag(msg->status, expect_flags)){
    printf("[Server]: Unexpected response from server\n");
    exit(EXIT_FAILURE);
  }
}

void signalHandler(){
  printf("\nTermination signal received. Closing socket\n");

  if(fd != -1){
    close(fd);
    fd = -1;
  }

  _exit(0);
}
