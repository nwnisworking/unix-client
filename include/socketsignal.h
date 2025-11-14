#ifndef SOCKETSIGNAL_H
#define SOCKETSIGNAL_H

/**
 * @file socketsignal.h
 * @brief Provides functions for installing signal handlers for socket operations.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/**
 * Installs custom signal handlers for termination signals.
 * 
 * @param handler Pointer to the signal handler function.
 */
void installSignalHandler(void* handler);

#endif // SOCKETSIGNAL_H