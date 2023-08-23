#include "handle_hash.h"

void generateHash(hashid hashid, char* data, unsigned char* hash, int length) {
    MHASH td;
    td = mhash_init(hashid);
    mhash(td, data, length);
    mhash_deinit(td, hash);
}

void writeHashToEndArray(hashid hashid, char* message, unsigned char* hash) {
    message += strlen(message);
    for (int i = 0; i < mhash_get_block_size(hashid); i++) {
        sprintf(message + 2 * i, "%.2x", hash[i]);
    }
}