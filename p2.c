#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

// BEGIN: Global Variables
enum { MAXN = 5 };
// END: Global Variables

// BEGIN: Shared Memory Variables

int SHM_KEY;

struct shmseg {
    int visitedRow[MAXN];
    int visitedCol[MAXN];
    int matrixOne[MAXN][MAXN];
    int matrixTwo[MAXN][MAXN];
    char outputFile[505];
};

int shmid;
struct shmseg* shmp;

// END: Shared Memory Variables

int main() {
    SHM_KEY = ftok("./p1.c", 0x2);

    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }

    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    printf("Output file is: %s\n", shmp->outputFile);

    // Only detatch and destroy after computing matrix product and printing

    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(-1);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(-1);
    }
}