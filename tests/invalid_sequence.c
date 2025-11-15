#include "test.h"

int main(){
  int fd = serverSocket(SERVER_PORT);
  Message msg;

  int client_fd = accept(fd, NULL, NULL);

  if(recvMessage(client_fd, &msg) != MSG_OK) ASSERT(0, "Failed to receive message");
  
  debugMessage(&msg);

  if(sendMessage(client_fd, RES_OK | PASSWORD, "Invalid sequence test") == MSG_OK){
    ASSERT(1, "Sent response after invalid sequence");
  }
  else{
    ASSERT(0, "Failed to send response after invalid sequence");
  }
}