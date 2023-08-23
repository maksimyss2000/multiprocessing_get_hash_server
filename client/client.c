#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mhash.h>

#define PORT 2000
#define ADDR "127.0.0.1"
#define START_SIZE_BUFFER 50
#define START_HEAD "POST /hash HTTP/1.0\n"\
                   "Accept: application/json\n"
#define START_JSON "{\"data\":\""
#define END_JSON  "\"}"

char* readMessageBySoket(int client_sd) {
    int size_buffer = START_SIZE_BUFFER;
    int reading_size = size_buffer;
    char* message = (char*)calloc(size_buffer + 1, sizeof(char));
    char* ptr = message;
    int count_read = 0;
    int index = 0;
    while ((count_read = read(client_sd, ptr, reading_size)) == reading_size) {
        size_buffer += reading_size;
        message = (char*)realloc(message, size_buffer + 1);
        index += count_read;
        ptr = message + index;
    }
    index += count_read;
    message[index] = '\0';
    return message;
}

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

void hanleRespone(char* response_message) {
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

int createSoket() {
    int server_sd;
    if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return server_sd;
}
  

void tryConnect(int client_fd, struct sockaddr_in* serv_addr) {
    int status;
    if ((status = connect(client_fd, (struct sockaddr*) serv_addr, sizeof(*serv_addr))) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

void setAddresSetting(struct sockaddr_in* serv_addr) {
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(PORT);
    serv_addr->sin_addr.s_addr = inet_addr(ADDR);
}

char* getDataByConsole() {
    char simbol;
    int current_size = 0;
    int max_size = START_SIZE_BUFFER;
    char* message = (char*) calloc(max_size + 1, sizeof(char));

    printf("data = ");
    while( (simbol=getchar()) != '\n') {
        if (current_size > max_size) {
            max_size *= 2;
            message = (char*)realloc(message, max_size);
        }
        message[current_size] = simbol;
        current_size += 1;
    }
    message[current_size] = '\0';
    return message;
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

int main(int argc, char const* argv[])
{
    struct sockaddr_in serv_addr;

    int client_fd = createSoket();
    setAddresSetting(&serv_addr);    
    tryConnect(client_fd, &serv_addr);

    char* data = getDataByConsole();
   // printf("%s\n", data);

    char* request_message = pacageMessage(data);
   // printf("%s\n", request_message);
    send(client_fd, request_message, strlen(request_message), 0);


    char* response_message = readMessageBySoket(client_fd);
    //printf("%s\n", response_message);

    
    hanleRespone(response_message);

    close(client_fd);
    free(data);
    free(request_message);
    free(response_message);
    return 0;
}