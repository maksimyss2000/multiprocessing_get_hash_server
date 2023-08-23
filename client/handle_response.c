#include "handle_responce.h"

#define START_HEAD "POST /hash HTTP/1.0\n"\
                   "Accept: application/json\n"
#define START_JSON "{\"data\":\""
#define END_JSON  "\"}"


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

char* pacageMessage(char* data) {
    int index = 0;
    int size_pacage_message = strlen(START_HEAD) + strlen(START_JSON) +
                              strlen(data)       + strlen(END_JSON)   + 1;
    char* pacage_message = (char*)calloc(size_pacage_message, sizeof(char));

    memcpy(pacage_message + index, START_HEAD, strlen(START_HEAD));
    index += strlen(START_HEAD);

    memcpy(pacage_message + index, START_JSON, strlen(START_JSON));
    index += strlen(START_JSON);

    memcpy(pacage_message + index, data,       strlen(data));
    index +=  strlen(data);

    memcpy(pacage_message + index, END_JSON,   strlen(END_JSON));
    index += strlen(END_JSON);
    
    pacage_message[index] = '\0';
    return pacage_message;
}
