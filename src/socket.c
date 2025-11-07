#include "socket.h"

int serverSocket(uint16_t port){
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  if(fd < 0) return -1;

  // Initialize address structure
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  // Bind the socket to the specified port
  if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return -1;

  // Start listening for incoming connections. This listens with a maximum backlog defined by the system.
  if(listen(fd, SOMAXCONN) < 0) return -1;

  return fd;
}

int clientSocket(const char* server_ip, uint16_t port){
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  if(fd < 0) return -1;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, server_ip, &addr.sin_addr);

  if(connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) return -1;

  return fd;
}

ssize_t sendMessage(int fd, uint8_t status, const char* data){
  int data_len = strlen(data);
  uint16_t net_length = htons(data_len);

  // Truncate data if it exceeds buffer size. 
  // This ensures we don't send more data than the receiver can handle.
  if(data_len > BUFFER_SIZE){
    data_len = BUFFER_SIZE; 
  }

  char msg_data[data_len + 3];

  // Construct the message: status (1 byte), length (2 bytes), data (n bytes)
  memcpy(&msg_data[0], &status, sizeof(uint8_t));
  memcpy(&msg_data[1], &net_length, sizeof(uint16_t));
  memcpy(&msg_data[3], data, data_len);
  
  // Null-terminate the data portion of the message
  if(msg_data[data_len + 3 - 1] == '\n')
    msg_data[data_len + 3 - 1] = '\0';

  return send(fd, msg_data, sizeof(msg_data), 0);
}

ssize_t recvMessage(int fd, Message* msg){
  // NULL pointer check
  if(!msg) return -1;

  ssize_t received_bytes = 0;
  uint16_t data_len;
  char header[3];

  // Clear the message structure in case the structure is being reused
  memset(msg, 0, sizeof(Message)); 

  // We want to prevent partial reads from happening, so we read the header first, then the data.
  while(received_bytes < (ssize_t) sizeof(header)){
    ssize_t bytes = recv(fd, header + received_bytes, sizeof(header) - received_bytes, 0);

    if(bytes <= 0){
      return -1; 
    }

    received_bytes += bytes;
  }

  // Extract status and length from header
  memcpy(&msg->status, &header[0], sizeof(uint8_t));
  memcpy(&msg->length, &header[1], sizeof(uint16_t));

  data_len = ntohs(msg->length);
  received_bytes = 0;

  while(received_bytes < data_len){
    ssize_t bytes = recv(fd, msg->data + received_bytes, data_len - received_bytes, 0);

    if(bytes <= 0){
      return -1;
    }

    received_bytes += bytes;
  }

  // Null-terminate the received data to make it a valid string
  // In case the data is exactly BUFFER_SIZE, we ensure the last byte is null
  if(data_len < BUFFER_SIZE)
    msg->data[data_len] = '\0';
  else
    msg->data[BUFFER_SIZE - 1] = '\0';
    
  return received_bytes;
}