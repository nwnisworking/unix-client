#include "test.h"

int main(){
  int fd = serverSocket(SERVER_PORT);
  Message msg;

  int client_fd = accept(fd, NULL, NULL);

  if(recvMessage(client_fd, &msg) != MSG_OK) ASSERT(0, "Failed to receive name message");

  if(sendMessage(client_fd, RES_OK | NAME, NULL) != MSG_OK) ASSERT(0, "Failed to send name ok message");

  if(recvMessage(client_fd, &msg) != MSG_OK) ASSERT(0, "Failed to receive password message");

  if(sendMessage(client_fd, RES_OK | PASSWORD, NULL) != MSG_OK) ASSERT(0, "Failed to send password ok message");

  if(sendMessage(client_fd, RES_OK | AUTH, "Authentication successful") == MSG_OK){
    ASSERT(1, "Sent authentication success message");
  }
  else{
    ASSERT(0, "Failed to send authentication failure message");
  }
}