#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

/*
  _____ _       _           _  __      __        _       _     _
 / ____| |     | |         | | \ \    / /       (_)     | |   | |
| |  __| | ___ | |__   __ _| |  \ \  / /_ _ _ __ _  __ _| |__ | | ___  ___
| | |_ | |/ _ \| '_ \ / _` | |   \ \/ / _` | '__| |/ _` | '_ \| |/ _ \/ __|
| |__| | | (_) | |_) | (_| | |    \  / (_| | |  | | (_| | |_) | |  __/\__ \
 \_____|_|\___/|_.__/ \__,_|_|     \/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
*/

int MAXTHREADS = 1;    // number of threads spawned

int I;    // Row size of matrix in in1.txt
int J;    // Col size of matrix in in1.txt = Row size of matrix in in2.txt
int K;    // Col size of matrix in in2.txt

char* inputFileOne;    // contains name of in1.txt
char* inputFileTwo;    // contains name of in2.txt
char* outputFile;      // contains name of out.txt

int* preprocessedMatrixOne;    // stores the pre-read matrix from in1.txt
int* preprocessedMatrixTwo;    // stores the pre-read matrix from in2.txt

pthread_t* threadID;    // stores ThreadIDs of MAXTHREADS threads

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
    int I;    // rows in matrixOne
    int J;    // columns in matrixOne == rows in matrixTwo
    int K;    // columns in matrixTwo
    char outputFile[100];
};
struct shmseg* shmp;

/*
    We are storing 2D matrices matrixOne and matrixTwo as linear arrays.

    If you have a 2D matrix of size (n X m), a coordinate (x, y) gets mapped to
    linear 1D array index (x * m + y).

    Similarly, a linear 1D array index of i gets mapped into 2D coordinates of
    (i/m, i%m)
*/

int* visitedRowOne;    // marks rows of matrixOne as read from in1.txt
int* visitedRowTwo;    // marks rows of matrixTwo as read from in2.txt
int* matrixOne;        // stores matrix read from in1.txt
int* matrixTwo;        // stores matrix read from in2.txt

void createSharedMemory() {
    int SHM_KEY, shmid;

    // First shared memory segment - stores shmseg
    SHM_KEY = ftok("./p1.c", 0x1);
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644 | IPC_CREAT);
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
    shmid = shmget(SHM_KEY, I * sizeof(int), 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    visitedRowOne = (int*) shmat(shmid, NULL, 0);
    if (visitedRowOne == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }
    memset(visitedRowOne, 0, I * sizeof(int));

    // Third shared memory segment - stores visitedRowTwo
    SHM_KEY = ftok("./p1.c", 0x3);
    shmid = shmget(SHM_KEY, J * sizeof(int), 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    visitedRowTwo = (int*) shmat(shmid, NULL, 0);
    if (visitedRowTwo == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }
    memset(visitedRowTwo, 0, J * sizeof(int));

    // Fourth shared memory segment - stores matrixOne
    SHM_KEY = ftok("./p1.c", 0x4);
    shmid = shmget(SHM_KEY, I * J * sizeof(int), 0644 | IPC_CREAT);
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
    shmid = shmget(SHM_KEY, J * K * sizeof(int), 0644 | IPC_CREAT);
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

void detachSharedMemory() {
    // Detaches from first shared memory segment
    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(-1);
    }

    // Detaches from second shared memory segment
    if (shmdt(visitedRowOne) == -1) {
        perror("shmdt");
        exit(-1);
    }

    // Detaches from third shared memory segment
    if (shmdt(visitedRowTwo) == -1) {
        perror("shmdt");
        exit(-1);
    }

    // Detaches from fourth shared memory segment
    if (shmdt(matrixOne) == -1) {
        perror("shmdt");
        exit(-1);
    }

    // Detaches from fifth shared memory segment
    if (shmdt(matrixTwo) == -1) {
        perror("shmdt");
        exit(-1);
    }
}

/*
 ______ _ _        _____                _ _
|  ____(_) |      |  __ \              | (_)
| |__   _| | ___  | |__) |___  __ _  __| |_ _ __   __ _
|  __| | | |/ _ \ |  _  // _ \/ _` |/ _` | | '_ \ / _` |
| |    | | |  __/ | | \ \  __/ (_| | (_| | | | | | (_| |
|_|    |_|_|\___| |_|  \_\___|\__,_|\__,_|_|_| |_|\__, |
                                                   __/ |
                                                  |___/
 ______                _   _
|  ____|              | | (_)
| |__ _   _ _ __   ___| |_ _  ___  _ __  ___
|  __| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
| |  | |_| | | | | (__| |_| | (_) | | | \__ \
|_|   \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
*/

void preprocessFileOne() {
    FILE* ptr = fopen(inputFileOne, "r");
    if (ptr == NULL) {
        printf("Error opening %s\n", inputFileOne);
        exit(-1);
    }
    int numRead = 0;
    int currentRow = 0;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while (numRead < I * J) {
        for (int i = 0; i < J; i++) {
            int num;
            fscanf(ptr, "%d", &num);
            preprocessedMatrixOne[currentRow * J + i] = num;
            ++numRead;
        }
        ++currentRow;
        read = getline(&line, &len, ptr);
    }
}

void preprocessFileTwo() {
    FILE* ptr = fopen(inputFileTwo, "r");
    if (ptr == NULL) {
        printf("Error opening %s\n", inputFileTwo);
        exit(-1);
    }
    int numRead = 0;
    int currentRow = 0;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while (numRead < J * K) {
        for (int i = 0; i < K; i++) {
            int num;
            fscanf(ptr, "%d", &num);
            preprocessedMatrixTwo[currentRow * K + i] = num;
            ++numRead;
        }
        ++currentRow;
        read = getline(&line, &len, ptr);
    }
}

void readRowsFromFileOne(int R) {
    for (int i = 0; i < J; i++) {
        matrixOne[R * J + i] = preprocessedMatrixOne[R * J + i];
    }
    visitedRowOne[R] = 1;
}

void readRowsFromFileTwo(int R) {
    for (int i = 0; i < K; i++) {
        matrixTwo[R * K + i] = preprocessedMatrixTwo[R * K + i];
    }
    visitedRowTwo[R] = 1;
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

/*
    Common thread runner function which implements the Skipgram approach for
    reading rows from the files.
    Assume that file 2 has 4 rows: [0, 1, 2, 3] and file 1 has 7 rows: [0, 1, 2, 3, 4, 5, 6]
    We first read from file 2: 0-3 and then from file 1: 0-6
    Therefore, the tasks can be represented as:
    Task: [0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6]
    ID:   [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    The first J tasks are row numbers of file 2, the next I tasks are file 1 row numbers.
    If thread id is t, then the thread performs task ID(s) [t, t+MAXTHREADS, t+2*MAXTHREADS ....]
*/
void* runner(void* arg) {
    int id = (uintptr_t) arg;
    while (id < I + J) {
        if (id < J) {
            readRowsFromFileTwo(id);
        } else {
            readRowsFromFileOne(id - J);
        }
        id += MAXTHREADS;
    }
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

int main(int argc, char* argv[]) {
    if (argc != 7 && argc != 8) {
        printf("Usage: ./p1 i j k in1.txt in2.txt out.txt [MAXTHREADS]\n");
        exit(-1);
    }

    I = atoi(argv[1]);
    J = atoi(argv[2]);
    K = atoi(argv[3]);
    preprocessedMatrixOne = (int*) malloc(I * J * sizeof(int));
    preprocessedMatrixTwo = (int*) malloc(J * K * sizeof(int));
    inputFileOne = argv[4];
    inputFileTwo = argv[5];
    outputFile = argv[6];
    if (argc == 8) {
        MAXTHREADS = atoi(argv[7]);
    }
    threadID = (pthread_t*) malloc(MAXTHREADS * sizeof(pthread_t));

    createSharedMemory();
    shmp->I = I;
    shmp->J = J;
    shmp->K = K;
    strcpy(shmp->outputFile, outputFile);

    preprocessFileOne();
    preprocessFileTwo();

    long long startTime = getCurrentTime();

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_create(&threadID[i], NULL, runner, (void*) (uintptr_t) i);
    }

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_join(threadID[i], NULL);
    }

    long long diff = getCurrentTime() - startTime;

    printf("%d,%lld\n", MAXTHREADS, diff);

    detachSharedMemory();
}
