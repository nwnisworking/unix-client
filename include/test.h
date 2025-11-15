#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "socket.h"
#include "protocol.h"

#define ASSERT(cond, msg) fprintf(stderr, "[Assertion %s]: %s for %s\n", (cond) ? "passed" : "failed", msg, __FILE_NAME__), exit(EXIT_SUCCESS);

/**
 * Checks if a specific flag is set in the status byte.
 * 
 * @param status The status byte to check.
 * @param flag The flag to check for.
 * @return 1 if the flag is set, 0 otherwise.
 */
int hasFlag(uint8_t status, uint8_t flag);

/**
 * Prints the details of a Message structure for debugging purposes.
 * 
 * @param msg Pointer to the Message structure to debug.
 * @return Always returns 1.
 */
int debugMessage(Message* msg);

#endif