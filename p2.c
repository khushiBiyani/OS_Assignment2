#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>

typedef long long ll;

/*
  _____ _       _           _  __      __        _       _     _
 / ____| |     | |         | | \ \    / /       (_)     | |   | |
| |  __| | ___ | |__   __ _| |  \ \  / /_ _ _ __ _  __ _| |__ | | ___  ___
| | |_ | |/ _ \| '_ \ / _` | |   \ \/ / _` | '__| |/ _` | '_ \| |/ _ \/ __|
| |__| | | (_) | |_) | (_| | |    \  / (_| | |  | | (_| | |_) | |  __/\__ \
 \_____|_|\___/|_.__/ \__,_|_|     \/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
*/

ll I;    // Row size of matrix in in1.txt
ll J;    // Col size of matrix in in1.txt == Col size of matrix in in2.txt
ll K;    // Row size of matrix in in2.txt
char outputFile[100];
ll MAXTHREADS = 1;
ll* outputMatrix;

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
    ll I;    // Row size of matrixOne
    ll J;    // Col size of matrixOne == Col size of matrixTwo
    ll K;    // Row size of matrixTwo
    char outputFile[100];
    bool sharedMemoryInitialized;
};
struct shmseg* shmp;
ll* visitedRowOne;    // marks rows of matrixOne as read from in1.txt
ll* visitedRowTwo;    // marks rows of matrixTwo as read from in2.txt
ll* matrixOne;        // stores matrix read from in1.txt
ll* matrixTwo;        // stores matrix read from in2.txt

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

    while (!shmp->sharedMemoryInitialized)
        ;

    // Second shared memory segment - stores visitedRowOne
    SHM_KEY = ftok("./p1.c", 0x2);
    shmid = shmget(SHM_KEY, shmp->I * sizeof(ll), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    visitedRowOne = (ll*) shmat(shmid, NULL, 0);
    if (visitedRowOne == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    // Third shared memory segment - stores visitedRowTwo
    SHM_KEY = ftok("./p1.c", 0x3);
    shmid = shmget(SHM_KEY, shmp->K * sizeof(ll), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    visitedRowTwo = (ll*) shmat(shmid, NULL, 0);
    if (visitedRowTwo == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    // Fourth shared memory segment - stores matrixOne
    SHM_KEY = ftok("./p1.c", 0x4);
    shmid = shmget(SHM_KEY, shmp->I * shmp->J * sizeof(ll), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    matrixOne = (ll*) shmat(shmid, NULL, 0);
    if (matrixOne == (void*) -1) {
        perror("Shared memory attach");
        exit(-1);
    }

    //  Fifth shared memory segment - stores matrixTwo
    SHM_KEY = ftok("./p1.c", 0x5);
    shmid = shmget(SHM_KEY, shmp->K * shmp->J * sizeof(ll), 0644);
    if (shmid == -1) {
        perror("Shared memory");
        exit(-1);
    }
    matrixTwo = (ll*) shmat(shmid, NULL, 0);
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
    shmid = shmget(SHM_KEY, I * sizeof(ll), 0644);
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
    shmid = shmget(SHM_KEY, K * sizeof(ll), 0644);
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
    shmid = shmget(SHM_KEY, I * J * sizeof(ll), 0644);
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
    shmid = shmget(SHM_KEY, K * J * sizeof(ll), 0644);
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
ll compute(ll row, ll col) {
    ll product = 0;

    ll offsetRowOne = row * J;
    ll offsetRowTwo = col * J;

    for (ll i = 0; i < J; i++) {
        product += matrixOne[offsetRowOne + i] * matrixTwo[offsetRowTwo + i];
    }

    return product;
}

void* runner(void* arg) {
    ll threadNumber = (uintptr_t) arg;
    if (threadNumber >= I * K) pthread_exit(NULL);
    ll numOfElements = ((I * K) + MAXTHREADS - 1) / MAXTHREADS;
    ll* elementsOfThread = (ll*) malloc(numOfElements * sizeof(ll));

    // initialising with -1
    for (ll i = 0; i < numOfElements; i++) {
        elementsOfThread[i] = -1;
    }

    ll myElementCount = 0;

    // finding each thread's elements and storing them in array
    for (ll x = 0; x < numOfElements; x++) {
        ll alloted = threadNumber + MAXTHREADS * x;
        if (alloted >= I * K) {
            break;
        }
        elementsOfThread[x] = alloted;
        ++myElementCount;
    }

    ll currCalculatedElements = 0;
    while (currCalculatedElements < myElementCount) {
        for (ll i = 0; i < numOfElements; i++) {
            ll cellNum = elementsOfThread[i];
            if (cellNum == -1) continue;
            ll row = cellNum / K;
            ll col = cellNum % K;
            if (visitedRowOne[row] && visitedRowTwo[col]) {
                outputMatrix[cellNum] = compute(row, col);
                elementsOfThread[i] = -1;
                ++currCalculatedElements;
            }
        }
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

int main(int argc, char* argv[]) {
    if (argc > 2) {
        printf("Usage: ./p2 [MAXTHREADS]\n");
        exit(-1);
    } else if (argc == 2) {
        MAXTHREADS = atoi(argv[1]);
    }

    connectSharedMemory();
    I = shmp->I;
    J = shmp->J;
    K = shmp->K;
    strcpy(outputFile, shmp->outputFile);

    outputMatrix = (ll*) malloc((I * K) * sizeof(ll));

    pthread_t* threadID;
    threadID = (pthread_t*) malloc(MAXTHREADS * sizeof(pthread_t));

    ll startTime = getCurrentTime();

    for (ll i = 0; i < MAXTHREADS; ++i) {
        pthread_create(&threadID[i], NULL, runner, (void*) (uintptr_t) i);
    }

    for (ll i = 0; i < MAXTHREADS; ++i) {
        pthread_join(threadID[i], NULL);
    }

    ll diff = getCurrentTime() - startTime;

    // printf("Matrix one size: %lld X %lld\n", I, J);
    // for (ll i = 0; i < I; ++i) {
    //     for (ll j = 0; j < J; ++j) {
    //         printf("%lld ", matrixOne[i * J + j]);
    //     }
    //     printf("\n");
    // }

    // printf("Matrix two size: %lld X %lld\n", K, J);
    // for (ll i = 0; i < K; ++i) {
    //     for (ll j = 0; j < J; ++j) {
    //         printf("%lld ", matrixTwo[i * J + j]);
    //     }
    //     printf("\n");
    // }

    // printf("Output matrix:\n");

    // for (ll i = 0; i < I; ++i) {
    //     for (ll j = 0; j < K; ++j) {
    //         printf("%lld ", outputMatrix[i * K + j]);
    //     }
    //     printf("\n");
    // }

    FILE* fpt = fopen("out.txt", "w");
    for (ll i = 0; i < I; ++i) {
        for (ll j = 0; j < K; ++j) {
            fprintf(fpt, "%lld ", outputMatrix[i * K + j]);
        }
        fprintf(fpt, "\n");
    }
    fclose(fpt);

    // Uncomment this block if you want to generate benchmark csv for p2.c
    // fpt = fopen("p2.csv", "a");
    // fprintf(fpt, "%lld,%lld\n", MAXTHREADS, diff);
    // fclose(fpt);

    free(outputMatrix);
    free(threadID);
    destroySharedMemory();
}
