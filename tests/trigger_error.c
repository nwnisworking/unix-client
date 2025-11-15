#include "test.h"

int main(){
  int fd = serverSocket(SERVER_PORT);
  Message msg;
  int client_fd = accept(fd, NULL, NULL);

  if(recvMessage(client_fd, &msg) != MSG_OK) ASSERT(0, "Failed to receive message");

  if(sendMessage(client_fd, RES_ERR, "Trigger error") == MSG_OK){
    ASSERT(1, "Error triggered successfully");
  }
  else{
    ASSERT(0, "Failed to trigger error");
  }
}