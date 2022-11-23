#include <pthread.h>
#include <stdbool.h>
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
int J;    // Col size of matrix in in1.txt == Col size of matrix in in2.txt
int K;    // Row size of matrix in in2.txt

char* inputFileOne;    // contains name of in1.txt
char* inputFileTwo;    // contains name of in2.txt
char* outputFile;      // contains name of out.txt

int* offsetFileOne;    // stores pre-processed row offsets for in1.txt
int* offsetFileTwo;    // stores pre-processed row offsets for in2.txt

pthread_mutex_t lock;     // lock as always
int currentRowOne = 0;    // Tracking to be read present row from in1.txt
int currentRowTwo = 0;    // Tracking to be read present row from in2.txt
bool parity = false;      // Tracking Parity to ensure RCRCRCCCC or RCRCRRRR execution to balance load

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

/*
    We are storing 2D matrices matrixOne and matrixTwo as linear arrays.

    If you have a 2D matrix of size (n X m), a coordinate (x, y) gets mapped to
    linear 1D array index (x * m + y).

    Similarly, a linear 1D array index of i gets mapped into 2D coordinates of
    (i/m, i%m)
*/

struct shmseg {
    int I;    // Row size of matrixOne
    int J;    // Col size of matrixOne == Col size of matrixTwo
    int K;    // Row size of matrixTwo
    char outputFile[100];
};
struct shmseg* shmp;

int* visitedRowOne;       // marks rows of matrixOne as read from in1.txt
int* visitedColumnTwo;    // marks rows of matrixTwo as read from in2.txt
int* matrixOne;           // stores matrix read from in1.txt
int* matrixTwo;           // stores matrix read from in2.txt

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

    // Third shared memory segment - stores visitedColumnTwo
    SHM_KEY = ftok("./p1.c", 0x3);
    shmid = shmget(SHM_KEY, K * sizeof(int), 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    visitedColumnTwo = (int*) shmat(shmid, NULL, 0);
    if (visitedColumnTwo == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }
    memset(visitedColumnTwo, 0, K * sizeof(int));

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
    shmid = shmget(SHM_KEY, K * J * sizeof(int), 0644 | IPC_CREAT);
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
    if (shmdt(visitedColumnTwo) == -1) {
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

void preProcessFileOne() {
    FILE* fptr = fopen(inputFileOne, "r");
    if (fptr == NULL) {
        printf("Error opening %s\n", inputFileOne);
        exit(-1);
    }

    char* line = NULL;
    size_t len = 0;
    size_t runningSum = 0;

    for (int i = 0; i < I; ++i) {
        offsetFileOne[i] = runningSum;
        getline(&line, &len, fptr);
        runningSum += strlen(line);
    }

    fclose(fptr);
}

void readRowsFromFileOne(int R) {
    FILE* fptr = fopen(inputFileOne, "r");
    if (fptr == NULL) {
        printf("Error opening %s\n", inputFileOne);
        exit(-1);
    }

    fseek(fptr, offsetFileOne[R], SEEK_SET);

    int num;
    for (int i = 0; i < J; i++) {
        fscanf(fptr, "%d", &num);
        matrixOne[R * J + i] = num;
    }
    visitedRowOne[R] = 1;

    fclose(fptr);
}

void preProcessFileTwo() {
    FILE* fptr = fopen(inputFileTwo, "r");
    if (fptr == NULL) {
        printf("Error opening %s\n", inputFileTwo);
        exit(-1);
    }

    char* line = NULL;
    size_t len = 0;
    size_t runningSum = 0;

    for (int i = 0; i < K; ++i) {
        offsetFileTwo[i] = runningSum;
        getline(&line, &len, fptr);
        runningSum += strlen(line);
    }

    fclose(fptr);
}

void readRowsFromFileTwo(int R) {
    FILE* fptr = fopen(inputFileTwo, "r");
    if (fptr == NULL) {
        printf("Error opening %s\n", inputFileTwo);
        exit(-1);
    }

    fseek(fptr, offsetFileTwo[R], SEEK_SET);

    for (int i = 0; i < J; i++) {
        int num;
        fscanf(fptr, "%d", &num);
        matrixTwo[R * J + i] = num;
    }
    visitedColumnTwo[R] = 1;

    fclose(fptr);
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

void* runner(void* arg) {
    int id = (uintptr_t) arg;
    while (currentRowOne < I || currentRowTwo < K) {
        pthread_mutex_lock(&lock);
        parity = !parity;
        if (parity) {
            if (currentRowOne < I) {
                readRowsFromFileOne(currentRowOne++);
            } else if (currentRowTwo < K) {
                readRowsFromFileTwo(currentRowTwo++);
            } else {
                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }
        } else {
            if (currentRowTwo < K) {
                readRowsFromFileTwo(currentRowTwo++);
            } else if (currentRowOne < I) {
                readRowsFromFileOne(currentRowOne++);
            } else {
                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }
        }
        pthread_mutex_unlock(&lock);
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
    inputFileOne = argv[4];
    inputFileTwo = argv[5];
    outputFile = argv[6];
    if (argc == 8) {
        MAXTHREADS = atoi(argv[7]);
    }

    pthread_t* threadID = (pthread_t*) malloc(MAXTHREADS * sizeof(pthread_t));
    offsetFileOne = (int*) malloc(I * sizeof(int));
    offsetFileTwo = (int*) malloc(K * sizeof(int));

    createSharedMemory();
    shmp->I = I;
    shmp->J = J;
    shmp->K = K;
    strcpy(shmp->outputFile, outputFile);

    preProcessFileOne();
    preProcessFileTwo();

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        exit(-1);
    }

    long long startTime = getCurrentTime();

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_create(&threadID[i], NULL, runner, (void*) (uintptr_t) i);
    }

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_join(threadID[i], NULL);
    }

    long long diff = getCurrentTime() - startTime;

    // Uncomment this block if you want to generate benchmark csv for p1.c
    // FILE* fpt = fopen("p1.csv", "a");
    // fprintf(fpt, "%d,%lld\n", MAXTHREADS, diff);
    // fclose(fpt);

    pthread_mutex_destroy(&lock);

    free(threadID);
    free(offsetFileOne);
    free(offsetFileTwo);

    detachSharedMemory();
}
