#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parsePesponse(char* request, char** hash_sha512, char** hash_gost);
void handleRespone(char* response_message);
char* pacageMessage(char* data);