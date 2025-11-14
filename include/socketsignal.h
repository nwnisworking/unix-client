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

/**
 * Installs custom signal handlers for termination signals.
 */
void installSignalHandler(void* handler);

#endif // SOCKETSIGNAL_H