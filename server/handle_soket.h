#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_SIZE_DATA 1024

int createSoket();
void configureStaringCharacteristics(int server_sd, struct sockaddr_in* address);
void startListen(int server_sd);
void acceptClient(int server_sd, int* client_sd);
char* readMessageBySoket(int client_sd);