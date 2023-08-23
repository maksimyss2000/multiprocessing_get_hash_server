#define MAX_COUNT_CONNECT 1000

#include "multiprocessing.h"
#include "handle_soket.h"


enum State {working, stoping}; 

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
    startListen(server_sd);

    while (work_status == working) {
        acceptClient(server_sd, &client_sd);
        sem_wait(semafore);
        createNewHandlerProcess(semafore, client_sd);
    }
    
    close(server_sd);
    sem_destroy(semafore);
    shmdt(semafore);
    shmctl(shmid,IPC_RMID,NULL);
    return 0;
}   