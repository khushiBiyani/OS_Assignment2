#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

// BEGIN: Global Variables

enum {
    MAXN = 5    // maximum dimension of any matrix
};

// END: Global Variables

// BEGIN: Shared Memory Variables

int SHM_KEY;

struct shmseg {
    int I;                        // rows in matrixOne
    int J;                        // columns in matrixOne == rows in matrixTwo
    int K;                        // columns in matrixTwo
    int visitedRowOne[MAXN];      // marks rows of matrixOne as read from in1.txt
    int visitedRowTwo[MAXN];      // marks rows of matrixTwo as read from in2.txt
    int matrixOne[MAXN][MAXN];    // stores matrix read from in1.txt
    int matrixTwo[MAXN][MAXN];    // stores matrix read from in2.txt
    char outputFile[505];
};

int shmid;              // stores the return value of shmget system call
struct shmseg* shmp;    // stores pointer to shared memory

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

    printf("Matrix one size: %d X %d\n", shmp->I, shmp->J);
    for (int i = 0; i < shmp->I; ++i) {
        for (int j = 0; j < shmp->J; ++j) {
            printf("%d ", shmp->matrixOne[i][j]);
        }
        printf("\n");
    }

    printf("Matrix two size: %d X %d\n", shmp->J, shmp->K);
    for (int j = 0; j < shmp->J; ++j) {
        for (int k = 0; k < shmp->K; ++k) {
            printf("%d ", shmp->matrixTwo[j][k]);
        }
        printf("\n");
    }

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