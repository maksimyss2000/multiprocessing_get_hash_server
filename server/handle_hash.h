#pragma once

#include <mhash.h>

void generateHash(hashid hashid, char* data, unsigned char* hash, int length);
void writeHashToEndArray(hashid hashid, char* message, unsigned char* hash);
