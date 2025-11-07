#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

// Default port number for server-client communication
#define SERVER_PORT 9000

// Maximum buffer size for message data
#define BUFFER_SIZE 65535 

// Status flags 
#define REQUEST_STATUS 1 << 7
#define RESPONSE_STATUS 1 << 6
#define SUCCESS_STATUS 1 << 5
#define ERROR_STATUS 1 << 4

// Message types
#define USERNAME 1
#define PASSWORD 2
#define DATA 3
#define CLOSE 4

// Message structure for client/server communication.
// This ensures both sides interpret the message consistently.
typedef struct {
  uint8_t status;
  uint16_t length;
  char data[BUFFER_SIZE];
} Message;

/**
 * Creates a server socket bound to the specified port and listens for incoming connections.
 * 
 * @param port The port number to bind the server socket to.
 * @return The file descriptor of the created server socket, or -1 on error.
 */
int serverSocket(uint16_t port);

/**
 * Creates a client socket and connects to the specified server IP and port.
 * 
 * @param server_ip The IP address of the server to connect to.
 * @param port The port number of the server to connect to.
 * @return The file descriptor of the created client socket, or -1 on error.
 */
int clientSocket(const char* server_ip, uint16_t port);

/**
 * Sends a message with the given status and data to the specified file descriptor.
 * 
 * @param fd The file descriptor to send the message to.
 * @param status The status byte of the message.
 * @param data The data to be sent in the message.
 * @return The number of bytes sent, or -1 on error.
 */
ssize_t sendMessage(int fd, uint8_t status, const char* data);

/**
 * Receives a message from the given file descriptor and populates the provided Message structure.
 * 
 * @param fd The file descriptor to receive the message from.
 * @param msg Pointer to the Message structure to populate with the received data.
 * @return The number of bytes received, or -1 on error.
 */
ssize_t recvMessage(int fd, Message* msg);