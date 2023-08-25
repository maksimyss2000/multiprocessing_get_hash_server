#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

int createSoket();
void tryConnect(int client_fd, struct sockaddr_in* serv_addr);
void setAddresSetting(struct sockaddr_in* serv_addr);
char* readMessageBySoket(int client_sd);