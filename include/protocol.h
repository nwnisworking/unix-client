#ifndef PROTOCOL_H
#define PROTOCOL_H

/**
 * @file protocol.h
 * @brief Defines the protocol for client-server communication, including message formats and status codes.
 */

#define SERVER_PORT 9000

// Request/Response flags
#define REQ_BIT 0 << 7
#define RES_BIT 1 << 7

// OK/Error flags
#define OK_BIT 0 << 6
#define ERR_BIT 1 << 6

// Status flags
#define REQ_OK (REQ_BIT | OK_BIT)
#define REQ_ERR (REQ_BIT | ERR_BIT)
#define RES_OK (RES_BIT | OK_BIT)
#define RES_ERR (RES_BIT | ERR_BIT)

// Message types
#define NAME 1
#define PASSWORD 2
#define AUTH 3
#define DATA 4
#define CLOSE 5

#endif