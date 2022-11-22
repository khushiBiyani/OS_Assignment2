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

int MAXTHREADS = 1;      // number of threads spawned
pthread_mutex_t lock;    // lock as always
int I;                   // Row size of matrix in in1.txt
int J;                   // Col size of matrix in in1.txt = Row size of matrix in in2.txt
int K;                   // Col size of matrix in in2.txt
int pRow = 0;            // Tracking to be read present row
int pCol = 0;            // Tracking to be read present column
bool parity = false;     // Tracking Parity to ensure RCRCRCCCC or RCRCRRRR execution to balance load
char* inputFileOne;      // contains name of in1.txt
char* inputFileTwo;      // contains name of in2.txt
char* outputFile;        // contains name of out.txt

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
        //printf("Error1");
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
        //printf("Error2");
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
    //printf("%d",SHM_KEY);
    //printf("J = %d\n",J);
    shmid = shmget(SHM_KEY, J * sizeof(int), 0644 | IPC_CREAT);
    if (shmid == -1) {
        //printf("Error3\n");
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
        //printf("Error4");
        perror("Shared memory");
        exit(-1);
    }
    matrixOne = (int*) shmat(shmid, NULL, 0);
    if (matrixOne == (void*) -1) {
        //printf("Error5");
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

void readRowsFromFileOne(int R) {
    FILE* ptr = fopen(inputFileOne, "r");
    if (ptr == NULL) {
        printf("Error opening %s\n", inputFileOne);
        exit(-1);
    }

    int rowCnt = 0;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while (rowCnt != R) {
        read = getline(&line, &len, ptr);
        rowCnt++;
    }

    int num;
    for (int i = 0; i < J; i++) {
        fscanf(ptr, "%d", &num);
        matrixOne[R * J + i] = num;
    }
    visitedRowOne[R] = 1;
}

void readRowsFromFileTwo(int R) {
    FILE* ptr = fopen(inputFileTwo, "r");
    if (ptr == NULL) {
        printf("Error opening %s\n", inputFileTwo);
        exit(-1);
    }

    int rowCnt = 0;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    while (rowCnt != R) {
        read = getline(&line, &len, ptr);
        rowCnt++;
    }

    int num;
    for (int i = 0; i < K; i++) {
        fscanf(ptr, "%d", &num);
        matrixTwo[R * K + i] = num;
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
void* runner(void* arg) {
    int id = (uintptr_t) arg;
    while (pRow < I || pCol < J) {
        
        pthread_mutex_lock(&lock);
        parity = !parity;
        if (parity) {
            if (pRow < I) {
                //printf("R %d \n", pRow);
                readRowsFromFileOne(pRow++);
            } else if (pCol < J) {
                //printf("C %d \n", pCol);
                readRowsFromFileTwo(pCol++);
            } else {
                pthread_mutex_unlock(&lock);
                pthread_exit(NULL);
            }
        } else {
            if (pCol < J) {
                //printf("C %d \n", pCol);
                readRowsFromFileTwo(pCol++);
            } else if (pRow < I) {
                //printf("R %d \n", pRow);
                readRowsFromFileOne(pRow++);
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
    // for(int i=0;i<argc;i++)
    //     printf("%s\n",argv[i]);
    I = atoi(argv[1]);
    J = atoi(argv[2]);
    K = atoi(argv[3]);
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
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    long long startTime = getCurrentTime();
    //printf("Starting creating threads\n");
    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_create(&threadID[i], NULL, runner, (void*) (uintptr_t) i);
    }

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_join(threadID[i], NULL);
    }

    long long diff = getCurrentTime() - startTime;

    //printf("%d,%lld\n", MAXTHREADS, diff);

    // Writing number of threads and time in nanoseconds into csv file

    FILE *fpt;
    fpt = fopen("./scripts/p1.csv", "a");

    fprintf(fpt,"%d, %lld\n", MAXTHREADS, diff);
    fclose(fpt);

    detachSharedMemory();
}
