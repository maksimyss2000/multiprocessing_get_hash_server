#pragma once 

#include <arpa/inet.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "multiprocessing.h"
#include "handle_request.h"

void createSemBySM(sem_t** semafore, int* shmid);
void handleEmergencyExit(int signal, siginfo_t *si, void *arg);
void handleSignal(sem_t* semafore);
void createNewHandlerProcess(sem_t* semafore, int client_sd);
