#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

// BEGIN: Global Variables

enum {
    MAXN = 5,          // maximum dimension of any matrix
    MAXTHREADS = 1,    // maximum number of threads
};

int i;    // Row size of matrix in in1.txt
int j;    // Col size of matrix in in1.txt = Row size of matrix in in2.txt
int k;    // Col size of matrix in in2.txt

char* inputFileOne;    // contains name of in1.txt
char* inputFileTwo;    // contains name of in2.txt
char* outputFile;

pthread_t threadID[MAXTHREADS];    // stores ThreadIDs of MAXTHREADS threads
int threadInfo[MAXTHREADS][3];     // stores information to be passed to each thread

int currentRow;    // stores row to be read from fileOne
int currentCol;    // stores column to be read from fileTwo
int parity;        // if parity == 0 -> read row from fileOne, else read column from fileTwo

// END: Global Variables

// BEGIN: Shared Memory Variables

int SHM_KEY;

struct shmseg {
    int visitedRow[MAXN];         // marks rows of matrix One as read from in1.txt
    int visitedCol[MAXN];         // marks columns of matrix Two as read from in2.txt
    int matrixOne[MAXN][MAXN];    // stores matrix read from in1.txt
    int matrixTwo[MAXN][MAXN];    // stores matrix read from in2.txt
    char outputFile[505];
};

int shmid;              // stores the return value of shmget system call
struct shmseg* shmp;    // stores pointer to shared memory

// END: Shared Memory Variables



void initialise(int curr[]){
    for(int p=0;p<MAXN;p++)
        curr[p]=0;
}

// BEGIN: Read rows from file in1.txt

void readRowsFromFileOne(int R) {
    // Reads row R of matrix in in1.txt into matrixOne[R][]
    FILE *ptr;
    ptr = fopen("in1.txt", "r");

    if(ptr == NULL) {
        printf("Error opening in1.txt\n");
        exit(-1);
    }

    int rowCnt = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while(rowCnt != R) {
        read = getline(&line, &len, ptr);
        rowCnt++;
    }
    
    // global : SHM_KEY, shmid, shmp


    long long num;
    for(int c = 0; c < j; c++) {
        fscanf(ptr, "%lld", &num);
        shmp->matrixOne[R][c] = num;
    }
    shmp->visitedRow[R]=1;
    // optimization -> preprocess file to calculate offset for every row
}

// END: Read rows from file in1.txt

// BEGIN: Read rows from file in1.txt

void readRowsFromFileTwo(int R) {
    // Reads row R of matrix in in1.txt into matrixOne[R][]
    FILE *ptr;
    ptr = fopen("in2.txt", "r");

    if(ptr == NULL) {
        printf("Error opening in2.txt\n");
        exit(-1);
    }

    int rowCnt = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while(rowCnt != R) {
        read = getline(&line, &len, ptr);
        rowCnt++;
    }
    
    // global : SHM_KEY, shmid, shmp


    long long num;
    for(int c = 0; c < k; c++) {
        fscanf(ptr, "%lld", &num);
        shmp->matrixTwo[R][c] = num;
    }
    shmp->visitedCol[R]=1;
    // optimization -> preprocess file to calculate offset for every row
}

// END: Read rows from file in1.txt


// BEGIN: Common Thread Runner Function

void* runner(void* arg) {
    // Common runner function for (i+k) threads
    int t = *((int*) arg);

    int currentRow = threadInfo[t][0];
    int currentCol = threadInfo[t][1];
    int parity = threadInfo[t][2];
    pthread_t tid = pthread_self();

    if (parity) {
        printf("[%lu] Reading Column: %d\n", tid, currentCol);
    } else {
        printf("[%lu] Reading Row: %d\n", tid, currentRow);
    }

    pthread_exit(NULL);
}

// END: Common Thread Runner Function


int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Usage: ./p1 i j k in1.txt in2.txt out.txt\n");
        exit(-1);
    }
    i = atoi(argv[1]);
    j = atoi(argv[2]);
    k = atoi(argv[3]);
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

    strcpy(shmp->outputFile, outputFile);

    initialise(shmp->visitedCol);
    initialise(shmp->visitedRow);

    for (int z = 0; z < i + k; ++z) {
        int pos;
        do {
            pos = findNextFreeThread();
        } while (pos == -1);
        threadInfo[pos][0] = currentRow;
        threadInfo[pos][1] = currentCol;
        threadInfo[pos][2] = parity;
        if (currentRow < i && currentCol < j) {
            if (parity) {
                ++currentCol;
            } else {
                ++currentRow;
            }
            parity ^= 1;
        } else if (currentRow < i) {
            ++currentRow;
        } else {
            ++currentCol;
        }
        pthread_create(&threadID[pos], NULL, runner, (void*) &pos);
    }

    for (int z = 0; z < MAXTHREADS; ++z) {
        pthread_join(threadID[z], NULL);
    }

    if (shmdt(shmp) == -1) {
        perror("shmdt");
        exit(-1);
    }
}

// int main(){

//     /*if (argc != 7) {
//         printf("Usage: ./p1 i j k in1.txt in2.txt out.txt\n");
//         exit(-1);
//     }
//     i = atoi(argv[1]);
//     j = atoi(argv[2]);
//     k = atoi(argv[3]);
//     inputFileOne = argv[4];
//     inputFileTwo = argv[5];
//     outputFile = argv[6];

//     SHM_KEY = ftok("./p1.c", 0x2);*/

//     i=4;
//     j=4;
//     k=3;

//     shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644 | IPC_CREAT);
//     if (shmid == -1) {
//         perror("Shared memory");
//         exit(-1);
//     }

//     shmp = shmat(shmid, NULL, 0);
//     if (shmp == (void*) -1) {
//         perror("Shared memory attach");
//         exit(-1);
//     }

//     strcpy(shmp->outputFile, outputFile);

//     initialise(shmp->visitedCol);
//     initialise(shmp->visitedRow);


//     readRowsFromFileTwo(2);
//     readRowsFromFileTwo(1);
//     readRowsFromFileTwo(0);
//     readRowsFromFileTwo(3);
//     readRowsFromFileOne(2);
//     readRowsFromFileOne(1);
//     readRowsFromFileOne(0);
//     readRowsFromFileOne(3);

//     for(int l=0;l<i;l++){
//         for(int m=0;m<j;m++)
//             printf("%d\t",shmp->matrixOne[l][m]);
//         printf("\n");
//     }
//     printf("\n");
//     for(int l=0;l<j;l++){
//         for(int m=0;m<k;m++)
//             printf("%d\t",shmp->matrixTwo[l][m]);
//         printf("\n");
//     }


//     if (shmdt(shmp) == -1) {
//         perror("shmdt");
//         exit(-1);
//     }

// }