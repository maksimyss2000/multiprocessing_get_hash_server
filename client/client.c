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

typedef struct Buffer {
    int max_size;
    int current_size;
    char* message;
} Buffer; 

printHash(unsigned char* sha512, unsigned char* gost) {
    for (int i = 0; i < mhash_get_block_size(MHASH_SHA512); i++) {
		printf("%.2x", sha512[i]);
	}
	printf("\n");
    	for (int i = 0; i < mhash_get_block_size(MHASH_GOST); i++) {
		printf("%.2x", gost[i]);
	}
	printf("\n");
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

void getMessageByConsole(Buffer* buf) {
    char simbol;
    while( (simbol=getchar()) != '\n' ) {
        if (buf->current_size + 1 >= buf->max_size) {
            buf->max_size *= 2;
            buf->message = (char*)realloc(buf->message, buf->max_size);
        }
        buf->message[buf->current_size] = simbol;
        buf->current_size += 1;
    }
    buf->message[buf->current_size] = '\0';
}

char* createPacageMessage(char* message) {
    int size_pacage_message = strlen(START_HEAD) + strlen(START_JSON) +
                              strlen(message)    + strlen(END_JSON)   + 1;
    char* pacage_message = (char*)calloc(size_pacage_message, sizeof(char));
    int index = 0;
    memcpy(pacage_message + index, START_HEAD, strlen(START_HEAD));
    index += strlen(START_HEAD);
    memcpy(pacage_message + index, START_JSON, strlen(START_JSON));
    index += strlen(START_JSON);
    memcpy(pacage_message + index, message   , strlen(message));
    index +=  strlen(message);
    memcpy(pacage_message + index, END_JSON  , strlen(END_JSON));
    index += strlen(END_JSON);
    pacage_message[index] = '\0';
    return pacage_message;
}

int main(int argc, char const* argv[])
{
    int valread;
    struct sockaddr_in serv_addr;
    Buffer buf;
    buf.current_size = 0;                        
    buf.max_size = START_SIZE_BUFFER;
    buf.message = (char*) calloc(buf.max_size, sizeof(char));

    int client_fd = createSoket();
    setAddresSetting(&serv_addr);
    getMessageByConsole(&buf);
    tryConnect(client_fd, &serv_addr);

    char* pacage_message = createPacageMessage(buf.message);
    printf("%s\n", pacage_message);
    send(client_fd, pacage_message, strlen(pacage_message), 0);
    printf("Hello message sent\n");
    valread = read(client_fd, buf.message, buf.max_size);
    buf.message[valread] = '\0';
    printf("(start)%s(end)\n", buf.message);
    close(client_fd);
    free(buf.message);
    free(pacage_message);
    return 0;
}