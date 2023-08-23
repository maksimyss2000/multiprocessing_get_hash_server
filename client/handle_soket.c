#include "handle_soket.h"

#define PORT 2000
#define ADDR "127.0.0.1"
#define START_SIZE_BUFFER 100

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