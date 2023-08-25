#pragma once

#include <semaphore.h>

#include "handle_hash.h"
#include "handle_soket.h"

void handleRequest(sem_t* semafore, int client_sd);