#include "handle_request.h"

#define START_HEAD "POST /hash HTTP/1.0\n"\
                   "Accept: application/json\n"
#define START_JSON "{\"data\":\""
#define END_JSON  "\"}"

char* pacageRequest(char* data) {
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


