#ifndef SOCKET_H
#define SOCKET_H

/**
 * @file socket.h
 * @brief Provides functions for creating server and client sockets, sending and receiving messages.
 */

#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

// Maximum buffer size for message data
#define BUFFER_SIZE 65535 

// Message status codes
#define MSG_OK 0
#define MSG_PEER_CLOSED 1
#define MSG_ERROR 2
#define MSG_MALFORMED 3

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
 * @return The status code indicating the result of the operation.
 */
int sendMessage(int fd, uint8_t status, const char* data);

/**
 * Receives a message from the given file descriptor and populates the provided Message structure.
 * 
 * @param fd The file descriptor to receive the message from.
 * @param msg Pointer to the Message structure to populate with the received data.
 * @return The status code indicating the result of the operation.
 */
int recvMessage(int fd, Message* msg);

#endif // SOCKET_H