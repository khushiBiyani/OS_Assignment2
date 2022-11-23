#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include<math.h>

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
int MAXTHREADS = 20;
int *outputMatrix;


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
 _______ _                        _   _____
|__   __| |                      | | |  __ \
   | |  | |__  _ __ ___  __ _  __| | | |__) |   _ _ __  _ __   ___ _ __
   | |  | '_ \| '__/ _ \/ _` |/ _` | |  _  / | | | '_ \| '_ \ / _ \ '__|
   | |  | | | | | |  __/ (_| | (_| | | | \ \ |_| | | | | | | |  __/ |
   |_|  |_| |_|_|  \___|\__,_|\__,_| |_|  \_\__,_|_| |_|_| |_|\___|_|

 ______                _   _
|  ____|              | | (_)
| |__ _   _ _ __   ___| |_ _  ___  _ __
|  __| | | | '_ \ / __| __| |/ _ \| '_ \
| |  | |_| | | | | (__| |_| | (_) | | | |
|_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|
*/

// multiply row and column
int compute(int row, int col) {
    int product = 0;
    int offsetRowOne = row * J;
    int offsetRowTwo = col * J;
    for(int i = 0; i < J; i++) {
        product += matrixOne[offsetRowOne+i] * matrixTwo[offsetRowTwo + i];
    }
    
    return product;
}


void* threadRunner(void* arg){

    int threadNumber = (uintptr_t) arg;
    int numOfElements=((I*K)+MAXTHREADS-1)/MAXTHREADS;
    int *elementsOfThread = (int *) malloc(numOfElements * sizeof(int));

    //initialising with -1
    for(int i=0;i<numOfElements;i++){
        elementsOfThread[i]=-1;
    }
    
    //finding each thread's elements and storing them in array
    for(int x=0;x<numOfElements;x++)
    {
        int alloted = threadNumber+MAXTHREADS*x;
        if(alloted >= I * K) {
            break;
        }
        elementsOfThread[x]=alloted;
    }
    
    int currCalculatedElements=0;
    // TODO : Loop Through, while loop, multiple threads
    for(int i = 0; i < numOfElements && currCalculatedElements<numOfElements; i++) {
        int cellNum = elementsOfThread[i];
        if(cellNum == -1) continue;
        int row = cellNum/K;
        int col = cellNum%K;
        // while(!visitedRowOne[row] || !visitedRowTwo[col]) {
        //     // waits, MAKE IT BETTER
        // }
        if(visitedRowOne[row] && visitedRowTwo[col]){
            outputMatrix[cellNum] = compute(row,col);
            currCalculatedElements+=1;
        }
        else if(i==numOfElements-1 && currCalculatedElements!=numOfElements)
            i = -1;
    }

    free(elementsOfThread);
    pthread_exit(NULL);
}

/*
  _______ _                                                      _
 |__   __(_)                                                    (_)
    | |   _ _ __ ___   ___   _ __ ___   ___  __ _ ___ _   _ _ __ _ _ __   __ _
    | |  | | '_ ` _ \ / _ \ | '_ ` _ \ / _ \/ _` / __| | | | '__| | '_ \ / _` |
    | |  | | | | | | |  __/ | | | | | |  __/ (_| \__ \ |_| | |  | | | | | (_| |
    |_|  |_|_| |_| |_|\___| |_| |_| |_|\___|\__,_|___/\__,_|_|  |_|_| |_|\__, |
                                                                          __/ |
                                                                         |___/
*/

static long long getCurrentTime(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (long long) ts.tv_sec * 1000000000ll + ts.tv_nsec;
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
    
    outputMatrix=(int *)malloc((I*K)*sizeof(int));

    // threads array
    pthread_t *threadID;
    threadID = (pthread_t*) malloc(MAXTHREADS * sizeof(pthread_t));

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

    long long startTime = getCurrentTime();

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_create(&threadID[i], NULL, threadRunner, (void*) (uintptr_t) i); 
    }

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_join(threadID[i], NULL);
    }

    printf("Output matrix:\n");

    for(int i=0;i<I;++i) {
        for(int j = 0; j < K; ++j) {
            printf("%d ", outputMatrix[i * K + j]);
        }
        printf("\n");
    }

    long long diff = getCurrentTime() - startTime;

    FILE* fpt = fopen("p2.csv", "a");
    fprintf(fpt, "%d,%lld\n", MAXTHREADS, diff);
    fclose(fpt);

    destroySharedMemory();
}
