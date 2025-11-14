#include "socket.h"

/**
 * Receives the full message from the given file descriptor and populates the provided buffer.
 * 
 * @param fd The file descriptor to receive the message from.
 * @param buffer The buffer to store the received data.
 * @param size The size of the buffer.
 * @param received_bytes Pointer to an integer to store the number of bytes received.
 * @return The status code indicating the result of the operation.
 */
static int readAll(int fd, char* buffer, int size, ssize_t* received_bytes);

/**
 * Sends the full buffer to the given file descriptor.
 * 
 * @param fd The file descriptor to send the data to.
 * @param buffer The buffer containing the data to send.
 * @param size The size of the buffer.
 * @param sent_bytes Pointer to an integer to store the number of bytes sent.
 * @return The status code indicating the result of the operation.
 */
static int sendAll(int fd, const char* buffer, int size, ssize_t* sent_bytes);

int serverSocket(uint16_t port){
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  if(fd < 0) return -1;

  // Initialize address structure
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  int opt = 1;
  // Reuse the address to avoid "Address already in use" errors on restart
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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

int sendMessage(int fd, uint8_t status, const char* data){
  ssize_t data_len = data ? strlen(data) : 0;

  // Truncate data if it exceeds buffer size. 
  // This ensures we don't send more data than the receiver can handle.
  if(data_len > BUFFER_SIZE) data_len = BUFFER_SIZE;

  // Ignore trailing newline if present
  if(data_len > 0 && data[data_len - 1] == '\n') data_len--;

  uint16_t net_length = htons(data_len);
  char msg_data[data_len + 3];
  ssize_t sent_bytes = 0;

  // Construct the message: status (1 byte), length (2 bytes), data (n bytes)
  memcpy(&msg_data[0], &status, sizeof(uint8_t));
  memcpy(&msg_data[1], &net_length, sizeof(uint16_t));

  if(data_len > 0) memcpy(&msg_data[3], data, data_len);

  if(sendAll(fd, msg_data, data_len + 3, &sent_bytes) != MSG_OK)
    return MSG_ERROR;

  if(sent_bytes != data_len + 3)
    return MSG_ERROR;

  return MSG_OK;
}

int recvMessage(int fd, Message* msg){
  // NULL pointer check
  if(!msg) return -1;

  ssize_t received_bytes = 0;
  uint16_t data_len;
  int status;
  char header[3];

  // Clear the message structure if it contains any residual data
  memset(msg, 0, sizeof(Message)); 

  // Receive the fixed-size header first (status + length)
  if((status = readAll(fd, header, 3, &received_bytes)) != MSG_OK) return status;

  // Extract status and length from header
  memcpy(&msg->status, &header[0], sizeof(uint8_t));
  memcpy(&msg->length, &header[1], sizeof(uint16_t));

  data_len = ntohs(msg->length);

  // Read the message data based on the length specified in the header
  if((status = readAll(fd, msg->data, data_len, &received_bytes)) != MSG_OK) return status;

  // Client might be sending data that does not match the declared length
  if(received_bytes != data_len){
    return MSG_MALFORMED;
  }

  // Null-terminate the received data.
  if(data_len < BUFFER_SIZE)
    msg->data[data_len] = '\0';
  else
    msg->data[BUFFER_SIZE - 1] = '\0';
    
  return MSG_OK;
}

int readAll(int fd, char* buffer, int size, ssize_t* received_bytes){
  *received_bytes = 0;

  while(*received_bytes < size){
    ssize_t bytes = recv(fd, buffer + *received_bytes, size - *received_bytes, 0);

    if(bytes == 0){
      return MSG_PEER_CLOSED;
    }
    else if(bytes < 0){
      return MSG_ERROR;
    }

    *received_bytes += bytes;
  }


  return MSG_OK;
}

int sendAll(int fd, const char* buffer, int size, ssize_t* sent_bytes){
  *sent_bytes = 0;

  while(*sent_bytes < size){
    ssize_t bytes = send(fd, buffer + *sent_bytes, size - *sent_bytes, 0);

    if(bytes <= 0){
      return MSG_ERROR;
    }

    *sent_bytes += bytes;
  }

  return MSG_OK;
}