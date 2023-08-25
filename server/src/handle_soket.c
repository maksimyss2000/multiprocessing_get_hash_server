#include "handle_soket.h"

#define PORT 2000
#define ADDR "127.0.0.1"
#define SIZE_REQUEST_WITHOUT_DATA 100
#define START_SIZE_BUFFER (MAX_SIZE_DATA + SIZE_REQUEST_WITHOUT_DATA)

int createSoket() {
    int server_sd;
    if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return server_sd;
}

void setSoketSetting(int server_sd) {
    int opt = 1;
    if (setsockopt(server_sd, SOL_SOCKET,
                   SO_REUSEADDR,
                   &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void bindAddressPortSoket(int server_fd, struct sockaddr_in* address) { 
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(ADDR);
    address->sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)address,
             sizeof(*address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}   

void printStatusWorkMessage() {
    printf("server has been started at %s:%d\n", ADDR, PORT);
}

void startListen(int server_sd) {
    if (listen(server_sd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printStatusWorkMessage();
}

void configureStaringCharacteristics(int server_sd, struct sockaddr_in* address){
    setSoketSetting(server_sd);
    bindAddressPortSoket(server_sd, address);
}

void acceptClient(int server_sd, int* client_sd) {
    if ((*client_sd = accept(server_sd, NULL, NULL)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}

//TODO: simplify the function. too wize
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
