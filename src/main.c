#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

#include "socket.h"
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
 * Handles server responses by receiving messages and checking for errors.
 */
void serverResponse(Message* msg);

int main(){
  atexit(cleanup);

  Message msg;
  char buffer[BUFFER_SIZE];
  char name[BUFFER_SIZE];

  fd = clientSocket("127.0.0.1", SERVER_PORT);

  if(fd < 0){
    printf("[Client]: Unable to connect to the server\n");
    exit(EXIT_FAILURE);
  }

  printf("[Client]: Connected to the server\n\n");
  // Install signal handlers to ensure the socket is closed on termination signals.
  installSignalHandler();

  printf("Enter username: ");
  if(!fgets(name, BUFFER_SIZE, stdin)) exit(EXIT_FAILURE);

  sendMessage(fd, REQUEST_STATUS | PASSWORD, name);
  serverResponse(&msg); // Server response to username

  name[strcspn(name, "\n")] = 0;

  printf("Enter password: ");
  if(!fgets(buffer, BUFFER_SIZE, stdin)) exit(EXIT_FAILURE);

  sendMessage(fd, REQUEST_STATUS | PASSWORD, buffer);
  serverResponse(&msg); // Server response to password

  serverResponse(&msg); // Welcome message if msg.status is SUCCESS_STATUS
  printf("[Server]: %s\n", msg.data);

  while(1){
    printf("[%s]: ", name);
    if(!fgets(buffer, BUFFER_SIZE, stdin)) exit(EXIT_FAILURE);

    if(strcmp(buffer, "exit\n") == 0){
      sendMessage(fd, REQUEST_STATUS | CLOSE, "");
      exit(EXIT_SUCCESS);
    }

    sendMessage(fd, REQUEST_STATUS | DATA, buffer);
    serverResponse(&msg);

    printf("[Server]: %s\n", msg.data);
  }

  return 0;
}

void cleanup(){
  printf("Cleaning up socket before exit...\n");

  if(fd >= 0){
    close(fd);
    fd = -1;
  }
}

void serverResponse(Message* msg){
  // Receive server response and check for errors
  if(recvMessage(fd, msg) < 0 || msg->status & ERROR_STATUS){
    printf("%s\n", msg->status & ERROR_STATUS ? msg->data : "Unable to reach the server");
    exit(EXIT_FAILURE);
  }
}