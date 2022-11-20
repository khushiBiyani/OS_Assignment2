#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

// BEGIN: Global Variables

enum {
    MAXN = 5,          // maximum dimension of any matrix
    MAXTHREADS = 2,    // maximum number of threads
};

int I;    // Row size of matrix in in1.txt
int J;    // Col size of matrix in in1.txt = Row size of matrix in in2.txt
int K;    // Col size of matrix in in2.txt

char* inputFileOne;    // contains name of in1.txt
char* inputFileTwo;    // contains name of in2.txt
char* outputFile;

pthread_t threadID[MAXTHREADS];    // stores ThreadIDs of MAXTHREADS threads

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

// BEGIN: Read rows from file in1.txt

void readRowsFromFileOne(int R) {
    // Reads row R of matrix in in1.txt into matrixOne[R][]
    FILE* ptr;
    ptr = fopen(inputFileOne, "r");

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
        shmp->matrixOne[R][i] = num;
    }
    shmp->visitedRowOne[R] = 1;
}

// END: Read rows from file in1.txt

// BEGIN: Read rows from file in2.txt

void readRowsFromFileTwo(int R) {
    // Reads row R of matrix in in2.txt into matrixTwo[R][]
    FILE* ptr;
    ptr = fopen(inputFileTwo, "r");

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
        shmp->matrixTwo[R][i] = num;
    }
    shmp->visitedRowTwo[R] = 1;
}

// END: Read rows from file in2.txt

// BEGIN: Common Thread Runner Function

void* runner(void* arg) {
    // Common runner function for MAXTHREADS threads
    int id = (int) arg;
    while(id<I+J){//Skipgram approach skipping in steps of MaxNumber of Threads
        if(id<J){// 0 1 2 3 0 1 2 3 4 5 6  We read first from file2 0 - 3 then from file1 0 to 6
            readRowsFromFileTwo(id);
        }
        else{//then from file1 0 to 6
            readRowsFromFileOne(id-J);
        }
        id+=MAXTHREADS;
    }
    pthread_exit(NULL);
}

// END: Common Thread Runner Function

int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Usage: ./p1 i j k in1.txt in2.txt out.txt\n");
        exit(-1);
    }
    I = atoi(argv[1]);
    J = atoi(argv[2]);
    K = atoi(argv[3]);
    inputFileOne = argv[4];
    inputFileTwo = argv[5];
    outputFile = argv[6];

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

    shmp->I = I;
    shmp->J = J;
    shmp->K = K;
    memset(shmp->visitedRowOne, 0, sizeof(shmp->visitedRowOne));
    memset(shmp->visitedRowTwo, 0, sizeof(shmp->visitedRowTwo));
    strcpy(shmp->outputFile, outputFile);

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_create(&threadID[i], NULL, runner, (void*) i);
    }

    for (int i = 0; i < MAXTHREADS; ++i) {
        pthread_join(threadID[i], NULL);
    }

    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(-1);
    }
}