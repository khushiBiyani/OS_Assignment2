#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

/*
  _____ _       _           _  __      __        _       _     _
 / ____| |     | |         | | \ \    / /       (_)     | |   | |
| |  __| | ___ | |__   __ _| |  \ \  / /_ _ _ __ _  __ _| |__ | | ___  ___
| | |_ | |/ _ \| '_ \ / _` | |   \ \/ / _` | '__| |/ _` | '_ \| |/ _ \/ __|
| |__| | | (_) | |_) | (_| | |    \  / (_| | |  | | (_| | |_) | |  __/\__ \
 \_____|_|\___/|_.__/ \__,_|_|     \/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
*/

int I;    // Row size of matrix in in1.txt
int J;    // Col size of matrix in in1.txt == Col size of matrix in in2.txt
int K;    // Row size of matrix in in2.txt
char outputFile[100];

/*
  _____ _                        _   __  __
 / ____| |                      | | |  \/  |
| (___ | |__   __ _ _ __ ___  __| | | \  / | ___ _ __ ___   ___  _ __ _   _
 \___ \| '_ \ / _` | '__/ _ \/ _` | | |\/| |/ _ \ '_ ` _ \ / _ \| '__| | | |
 ____) | | | | (_| | | |  __/ (_| | | |  | |  __/ | | | | | (_) | |  | |_| |
|_____/|_| |_|\__,_|_|  \___|\__,_| |_|  |_|\___|_| |_| |_|\___/|_|   \__, |
                                                                       __/ |
                                                                      |___/
*/

struct shmseg {
    int I;    // Row size of matrixOne
    int J;    // Col size of matrixOne == Col size of matrixTwo
    int K;    // Row size of matrixTwo
    char outputFile[100];
};
struct shmseg* shmp;
int* visitedRowOne;    // marks rows of matrixOne as read from in1.txt
int* visitedRowTwo;    // marks rows of matrixTwo as read from in2.txt
int* matrixOne;        // stores matrix read from in1.txt
int* matrixTwo;        // stores matrix read from in2.txt

void connectSharedMemory() {
    int SHM_KEY, shmid;

    // First shared memory segment - stores shmseg
    SHM_KEY = ftok("./p1.c", 0x1);
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    shmp = (struct shmseg*) shmat(shmid, NULL, 0);
    if (shmp == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    // Second shared memory segment - stores visitedRowOne
    SHM_KEY = ftok("./p1.c", 0x2);
    shmid = shmget(SHM_KEY, shmp->I * sizeof(int), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    visitedRowOne = (int*) shmat(shmid, NULL, 0);
    if (visitedRowOne == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    // Third shared memory segment - stores visitedRowTwo
    SHM_KEY = ftok("./p1.c", 0x3);
    shmid = shmget(SHM_KEY, shmp->K * sizeof(int), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    visitedRowTwo = (int*) shmat(shmid, NULL, 0);
    if (visitedRowTwo == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    // Fourth shared memory segment - stores matrixOne
    SHM_KEY = ftok("./p1.c", 0x4);
    shmid = shmget(SHM_KEY, shmp->I * shmp->J * sizeof(int), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    matrixOne = (int*) shmat(shmid, NULL, 0);
    if (matrixOne == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    //  Fifth shared memory segment - stores matrixTwo
    SHM_KEY = ftok("./p1.c", 0x5);
    shmid = shmget(SHM_KEY, shmp->K * shmp->J * sizeof(int), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    matrixTwo = (int*) shmat(shmid, NULL, 0);
    if (matrixTwo == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }
}

void destroySharedMemory() {
    // Destroys first shared memory segment
    int SHM_KEY = ftok("./p1.c", 0x1);
    int shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644);
    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(-1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(-1);
    }

    // Destroys second shared memory segment
    SHM_KEY = ftok("./p1.c", 0x2);
    shmid = shmget(SHM_KEY, I * sizeof(int), 0644);
    if (shmdt(visitedRowOne) == -1) {
        perror("shmdt");
        exit(-1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(-1);
    }

    // Destroys third shared memory segment
    SHM_KEY = ftok("./p1.c", 0x3);
    shmid = shmget(SHM_KEY, K * sizeof(int), 0644);
    if (shmdt(visitedRowTwo) == -1) {
        perror("shmdt");
        exit(-1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(-1);
    }

    // Destroys fourth shared memory segment
    SHM_KEY = ftok("./p1.c", 0x4);
    shmid = shmget(SHM_KEY, I * J * sizeof(int), 0644);
    if (shmdt(matrixOne) == -1) {
        perror("shmdt");
        exit(-1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(-1);
    }

    // Destroys fifth shared memory segment
    SHM_KEY = ftok("./p1.c", 0x5);
    shmid = shmget(SHM_KEY, K * J * sizeof(int), 0644);
    if (shmdt(matrixTwo) == -1) {
        perror("shmdt");
        exit(-1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl");
        exit(-1);
    }
}

/*
 __  __       _         ______                _   _
|  \/  |     (_)       |  ____|              | | (_)
| \  / | __ _ _ _ __   | |__ _   _ _ __   ___| |_ _  ___  _ __
| |\/| |/ _` | | '_ \  |  __| | | | '_ \ / __| __| |/ _ \| '_ \
| |  | | (_| | | | | | | |  | |_| | | | | (__| |_| | (_) | | | |
|_|  |_|\__,_|_|_| |_| |_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|
*/

int main() {
    connectSharedMemory();
    I = shmp->I;
    J = shmp->J;
    K = shmp->K;
    strcpy(outputFile, shmp->outputFile);

    printf("Matrix one size: %d X %d\n", I, J);
    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j) {
            printf("%d ", matrixOne[i * J + j]);
        }
        printf("\n");
    }

    printf("Matrix two size: %d X %d\n", K, J);
    for (int i = 0; i < K; ++i) {
        for (int j = 0; j < J; ++j) {
            printf("%d ", matrixTwo[i * J + j]);
        }
        printf("\n");
    }

    destroySharedMemory();
}
