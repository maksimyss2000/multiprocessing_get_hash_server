#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <assert.h>
#include <mhash.h>

#define PORT 2000
#define ADDR "127.0.0.1"
#define MAX_COUNT_CONNECT 2
#define FORK_CHILD_PID 0
#define MAX_SIZE_DATA 1024
#define SIZE_REQUEST_WITHOUT_DATA 200
#define START_SIZE_BUFFER (MAX_SIZE_DATA + SIZE_REQUEST_WITHOUT_DATA)

enum State {working, stoping}; 

typedef struct FieldsRequest {
    char* method;
    char* url;
    char* version;
    char* type_accept;
    char* data;
} FieldsRequest; 


void generateHash(hashid hashid, char* data, unsigned char* hash, int length) {
    MHASH td;
    td = mhash_init(hashid);
    mhash(td, data, length);
    mhash_deinit(td, hash);
}

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

void startListen(int server_sd) {
    if (listen(server_sd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void configureStaringCharacteristics(int server_sd, struct sockaddr_in* address){
    setSoketSetting(server_sd);
    bindAddressPortSoket(server_sd, address);
    startListen(server_sd);
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

//TODO:it's a temporary solution that's not entirely correct. 
//     A full-fledged json parser is needed
//     a test that won't pass {"data":"12\"3"}
void parserPequest(char* request, FieldsRequest* fields_request) {
    char* ptr_data_field = strstr(request, "\"data\"");
    char* method = strtok(request, " ");
    char* url = strtok(NULL, " ");
    char* version = strtok(NULL, " \n");
    strtok(NULL, " ");                     /*skip "Accept:"*/
    char* type_accept = strtok(NULL, " \n");
    char* data = strtok(ptr_data_field + strlen("\"data\""), ":\"");

    assert(strlen(data) <= MAX_SIZE_DATA);
    assert(method != NULL);
    assert(url != NULL);
    assert(version != NULL);
    assert(type_accept  != NULL);

    fields_request->method = method;
    fields_request->url = url;
    fields_request->version = version;
    fields_request->type_accept = type_accept;
    fields_request->data = data;
}

void handlerPostHash(int client_sd, char* data) {
    unsigned char sha512[512];
    unsigned char gost[256];
    generateHash(MHASH_SHA512, data, sha512, strlen(data));
    generateHash(MHASH_GOST, data, gost, strlen(data));
}

void handlerNotFound(int client_sd) {
    char header[] = "HTTP/1.0 404 Not Found";
    send(client_sd, header, strlen(header), 0);
}

void route(int client_sd, FieldsRequest* fields_request) {
    int status_code;
    if ((!strcmp("POST", fields_request->method)) && (!strcmp("/hash", fields_request->url))) {
        handlerPostHash(client_sd, fields_request->data);
    } else {
        handlerNotFound(client_sd);
    }
}

void handleRequest(sem_t* semafore, int client_sd) {
    FieldsRequest fields_request;
    char* request = readMessageBySoket(client_sd);
   // printf("(start)%s(end)\n", request);
    parserPequest(request, &fields_request);
    route(client_sd, &fields_request);
    sem_post(semafore);
    free(request);
    exit(0);
}
#include <signal.h>


//TODO:dont forget to move in file
sem_t* gloabal_ptr_semafore;

void handleEmergencyExit(int signal, siginfo_t *si, void *arg) {
    sem_post(gloabal_ptr_semafore);
    exit(0);
}

/* Instead of a try-finally . 
*  If the programme is stopped in an emergency(ex segfault), the semaphore must be posted.
*/
void handlerSignal(sem_t* semafore) {
    gloabal_ptr_semafore = semafore;

    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handleEmergencyExit;
    sa.sa_flags   = SA_SIGINFO;

    sigaction(SIGSEGV, &sa, NULL);
}

void createNewHandlerProcess(sem_t* semafore, int client_sd) {
    if (fork() == FORK_CHILD_PID) {
        handlerSignal(semafore);
        handleRequest(semafore, client_sd);
    }
}

void createSemBySM(sem_t** semafore, int* shmid) {
    key_t key = ftok("shmfile", 65);
    *shmid = shmget(key, sizeof(sem_t), 0666|IPC_CREAT);
    if (*shmid < 0) { 
        perror ("shmget");
        exit (1);
    }
    *semafore = (sem_t*) shmat(*shmid, (void*)0, 0);
}

void printStatusWorkMessage() {
    printf("server has been started at %s:%d\n", ADDR, PORT);
}

int main() {
    int client_sd;
    int shmid;
    sem_t* semafore = NULL;
    struct sockaddr_in address;
    
    enum State work_status = working;    

    int server_sd = createSoket();

    createSemBySM(&semafore, &shmid); 
    sem_init(semafore, 1, MAX_COUNT_CONNECT);
    configureStaringCharacteristics(server_sd, &address);
    printStatusWorkMessage();

    while (work_status == working) {
        acceptClient(server_sd, &client_sd);
        sem_wait(semafore);
        createNewHandlerProcess(semafore, client_sd);
    }

    sem_destroy(semafore);
    shmdt(semafore);
    shmctl(shmid,IPC_RMID,NULL);
    close(server_sd);
    return 0;
}   