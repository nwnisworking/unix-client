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

// This is a global file descriptor for the socket 
// which can either be a server or accepted client socket.
// The purpose of this global variable is to allow signal handlers to close the socket gracefully.
int fd = -1;

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
 * Checks if a specific flag is set in the status byte.
 * @param status The status byte to check.
 * @param flag The flag to check for.
 * @return 1 if the flag is set, 0 otherwise.
 */
int hasFlag(uint8_t status, uint8_t flag);

int main(){
  atexit(cleanup);
  installSignalHandler();

  Message msg;
  char buffer[BUFFER_SIZE];
  char name[BUFFER_SIZE];
  struct timeval timeout = {15, 0};

  fd = clientSocket("127.0.0.1", SERVER_PORT);

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

int hasFlag(uint8_t status, uint8_t flag){
  return (status & flag) == flag;
}