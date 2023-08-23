#include "multiprocessing.h"

#define FORK_CHILD_PID 0

sem_t* gloabal_ptr_semafore;

void createSemBySM(sem_t** semafore, int* shmid) {
    key_t key = ftok("shmfile", 65);
    *shmid = shmget(key, sizeof(sem_t), 0666|IPC_CREAT);
    if (*shmid < 0) { 
        perror ("shmget");
        exit (1);
    }
    *semafore = (sem_t*) shmat(*shmid, (void*)0, 0);
}

void handleEmergencyExit(int signal, siginfo_t *si, void *arg) {
    sem_post(gloabal_ptr_semafore);
    exit(0);
}

/* Instead of a try-finally . 
*  If the programme is stopped in an emergency(ex segfault), the semaphore must be posted.
*/
void handleSignal(sem_t* semafore) {
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
        handleSignal(semafore);
        handleRequest(semafore, client_sd);
    }
}

