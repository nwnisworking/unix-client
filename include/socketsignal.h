#ifndef SOCKETSIGNAL_H
#define SOCKETSIGNAL_H

/**
 * @file socketsignal.h
 * @brief Provides functions for installing and resetting signal handlers for socket operations.
 */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// This variable is defined in main.c
// Since the program may run as either server or client,
// we use a single global file descriptor to manage the active connection.
// By declaring it extern here, we can access it in socketsignal.c for signal handling.
// This allows us to close the socket gracefully on termination signals.
// calls.
extern int fd;

/**
 * Resets the signal handlers to their default behavior.
 */
void resetSignalHandler();

/**
 * Installs custom signal handlers for termination signals.
 */
void installSignalHandler();

#endif // SOCKETSIGNAL_H