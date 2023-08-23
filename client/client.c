#include <stdio.h>
#include <string.h>

#include "handle_soket.h"
#include "handle_responce.h"

#define START_SIZE_CONSOLE_BUFFER 100

char* getDataByConsole() {
    char simbol;
    int current_size = 0;
    int max_size = START_SIZE_CONSOLE_BUFFER;
    char* message = (char*) calloc(max_size + 1, sizeof(char));

    printf("data = ");
    while( (simbol=getchar()) != '\n') {
        if (current_size > max_size) {
            max_size *= 2;
            message = (char*)realloc(message, max_size + 1);
        }
        message[current_size] = simbol;
        current_size += 1;
    }
    message[current_size] = '\0';
    return message;
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
    
    handleRespone(response_message);

    close(client_fd);
    free(data);
    free(request_message);
    free(response_message);
    return 0;
}