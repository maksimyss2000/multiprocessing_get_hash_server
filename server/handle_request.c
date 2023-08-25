#include "handle_request.h"

#define START_HEAD "POST 200 HTTP/1.0\n"\
                   "Accept: application/json\n"

typedef struct FieldsRequest {
    char* method;
    char* url;
    char* version;
    char* type_accept;
    char* data;
} FieldsRequest; 


void testParseRequest(FieldsRequest* fields_request) {
    assert(strlen(fields_request->data) <= MAX_SIZE_DATA);
    assert(fields_request->method != NULL);
    assert(fields_request->url != NULL);
    assert(fields_request->version != NULL);
    assert(fields_request->type_accept  != NULL);
}

//TODO:it's a temporary solution that's not entirely correct. 
//     A full-fledged json parser is needed
//     a test that won't pass {"data":"12\"3"}
void parsePequest(char* request, FieldsRequest* fields_request) {
    char* ptr_data_field = strstr(request, "\"data\"");
    char* method = strtok(request, " ");
    char* url = strtok(NULL, " ");
    char* version = strtok(NULL, " \n");
    strtok(NULL, " ");                     /*skip "Accept:"*/
    char* type_accept = strtok(NULL, " \n");
    char* data = strtok(ptr_data_field + strlen("\"data\""), ":\"");

    fields_request->method = method;
    fields_request->url = url;
    fields_request->version = version;
    fields_request->type_accept = type_accept;
    fields_request->data = data;

    //testParseRequest(fields_request);
}

void createJson(char* pacage_message, unsigned char* sha512, unsigned char* gost) {
    int index = strlen(pacage_message);

    char start_json[] = "{\"sha512\":";
    memcpy(pacage_message + index, start_json, strlen(start_json));
    index +=  strlen(start_json);

    writeHashToEndArray(MHASH_SHA512, pacage_message, sha512);
    index += mhash_get_block_size(MHASH_SHA512) * 2;

    char middle_json[] = "\",\n\"gost\":\"";
    memcpy(pacage_message + index, middle_json, strlen(middle_json));
    index +=  strlen(middle_json);

    writeHashToEndArray(MHASH_GOST, pacage_message, gost);
    index += mhash_get_block_size(MHASH_GOST) * 2;

    char end_json[] = "\"}";
    memcpy(pacage_message + index, end_json, strlen(end_json));
    index +=  strlen(end_json);

    pacage_message[index] = '\0';
}

char* createPacageMessage(unsigned char* sha512, unsigned char* gost) {
    int size_pacage_message = strlen(START_HEAD)
                             + mhash_get_block_size(MHASH_SHA512) * 2
                             + mhash_get_block_size(MHASH_GOST) * 2
                             + 100;        // for json
    char* pacage_message = (char*)calloc(size_pacage_message, sizeof(char));
    memcpy(pacage_message, START_HEAD, strlen(START_HEAD));
    createJson(pacage_message, sha512, gost);
    return pacage_message;
}

void handlePostHash(int client_sd, char* data) {
    unsigned char sha512[512];
    unsigned char gost[256];
    generateHash(MHASH_SHA512, data, sha512, strlen(data));
    generateHash(MHASH_GOST, data, gost, strlen(data));
    
    char* pacage_message = createPacageMessage(sha512, gost);
    send(client_sd, pacage_message, strlen(pacage_message), 0);

    free(pacage_message);
}   

void handleNotFound(int client_sd) {
    char header[] = "HTTP/1.0 404 Not Found";
    send(client_sd, header, strlen(header), 0);
}

void route(int client_sd, FieldsRequest* fields_request) {
    int status_code;
    if ((!strcmp("POST", fields_request->method)) && (!strcmp("/hash", fields_request->url))) {
        handlePostHash(client_sd, fields_request->data);
    } else {
        handleNotFound(client_sd);
    }
}

void handleRequest(sem_t* semafore, int client_sd) {
    FieldsRequest fields_request;
    char* request = readMessageBySoket(client_sd);

    parsePequest(request, &fields_request);
    route(client_sd, &fields_request);

    sem_post(semafore);
    free(request);
    exit(0);
}
