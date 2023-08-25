#include "handle_responce.h"

int parsePesponse(char* request, char** hash_sha512, char** hash_gost) {
    char* field_sha512 = strstr(request, "\"sha512\"");
    char* field_gost = strstr(request, "\"gost\"");
    char* method = strtok(request, " ");
    char* status = strtok(NULL, " ");
    char* version = strtok(NULL, " \n");
    if (strcmp(status, "200")) {
        return atoi(status);
    }

    strtok(NULL, " ");                     /*skip "Accept:"*/
    char* type_accept = strtok(NULL, " \n");

    *hash_sha512 = strtok(field_sha512 + strlen("\"sha512\""), ":\"");
    *hash_gost  = strtok(field_gost + strlen("\"gost\""), ":\"");
    return atoi(status);
}

void handleRespone(char* response_message) {
    char* hash_sha512;
    char* hash_gost;
    int code = parsePesponse(response_message, &hash_sha512, &hash_gost);
    if (code == 200) {
        printf("hash_sha512(data) = %s\n", hash_sha512);
        printf("hash_gost(data) = %s\n", hash_gost);
    } else {
        printf("response error %d\n", code);
    }
}

