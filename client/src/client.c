#include <stdio.h>
#include <string.h>

#include "handle_soket.h"
#include "handle_responce.h"
#include "handle_request.h"

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

    char* request = pacageRequest(data);
   // printf("%s\n", request);
    send(client_fd, request, strlen(request), 0);

    char* response = readMessageBySoket(client_fd);
    //printf("%s\n", response);
    
    handleRespone(response);

    close(client_fd);
    free(data);
    free(request);
    free(response);
    return 0;
}