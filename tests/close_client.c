#include "test.h"

int main(){
  int fd = serverSocket(SERVER_PORT);
  Message msg;

  int client_fd = accept(fd, NULL, NULL);

  if(recvMessage(client_fd, &msg) != MSG_OK) ASSERT(0, "Failed to receive name message");

  if(sendMessage(client_fd, RES_OK | NAME, NULL) != MSG_OK) ASSERT(0, "Failed to send name ok message");

  if(recvMessage(client_fd, &msg) != MSG_OK) ASSERT(0, "Failed to receive password message");

  if(sendMessage(client_fd, RES_OK | PASSWORD, NULL) != MSG_OK) ASSERT(0, "Failed to send password ok message");

  if(sendMessage(client_fd, RES_OK | AUTH, "Authentication Success") != MSG_OK){
    ASSERT(0, "Unable to send authentication success message");
  }

  while(1){
    if(recvMessage(client_fd, &msg) != MSG_OK) ASSERT(0, "Failed to receive data message");
    sendMessage(client_fd, RES_OK | DATA, msg.data);
    if(hasFlag(msg.status, CLOSE)){
      ASSERT(1, "Received close message from client");
      break;
    }
  }
}