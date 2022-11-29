#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>


#define PROCESS_ALIVE 0x141221

static long long getCurrentTime(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (long long) ts.tv_sec * 1000000000ll + ts.tv_nsec;
}

int main(int argc, char* argv[]) {
    FILE *fptr;
    long long start_time=0,end_time=0;
    long long diff=0,count=0;
    if (argc != 7 && argc != 8) {
        printf("Usage: ./sched i j k in1.txt in2.txt out.txt [MAXTHREADS]\n");
        exit(-1);
    }
    char* I = argv[1];
    char* J = argv[2];
    char* K = argv[3];
    char* inputFileOne = argv[4];
    char* inputFileTwo = argv[5];
    char* outputFile = argv[6];
    char* MAXTHREADS = "1";
    if (argc == 8) {
        MAXTHREADS = argv[7];
    }

    pid_t processOnePid = -1;
    pid_t processTwoPid = -1;
    pid_t schedulerPid = getpid();

    processOnePid = fork();    // returns 0 to child and child-pid to parent.
    if (!processOnePid) {
        execlp("./p1", "./p1", I, J, K, inputFileOne, inputFileTwo, outputFile, MAXTHREADS, NULL);
    }
    kill(processOnePid, SIGSTOP);    // we are NOT killing the processes.

    processTwoPid = fork();
    if (!processTwoPid) {
        execlp("./p2", "./p2", MAXTHREADS, NULL);
    }
    kill(processTwoPid, SIGSTOP);

    bool turn = 0;
    int processOneAlive = PROCESS_ALIVE;
    int processTwoAlive = PROCESS_ALIVE;
    fptr = fopen("contextSwitch.txt", "w");

    while (1) {
        start_time=getCurrentTime();
        waitpid(processOnePid, &processOneAlive, WNOHANG);
        waitpid(processTwoPid, &processTwoAlive, WNOHANG);
        if (processOneAlive == PROCESS_ALIVE && processTwoAlive == PROCESS_ALIVE) {
            if (turn & 1) {
                kill(processOnePid, SIGSTOP);
                kill(processTwoPid, SIGCONT);
            } else {
                kill(processTwoPid, SIGSTOP);
                kill(processOnePid, SIGCONT);
            }
        } else if (processOneAlive == PROCESS_ALIVE && processTwoAlive != PROCESS_ALIVE) {
            kill(processOnePid, SIGCONT);
        } else if (processOneAlive != PROCESS_ALIVE && processTwoAlive == PROCESS_ALIVE) {
            kill(processTwoPid, SIGCONT);
        } else {
            break;
        }
        turn ^= 1;
        usleep(2000);
        end_time=getCurrentTime();
        count++;
        diff+=end_time-start_time;
    }
    fprintf(fptr, "%lld ", diff/count);
    fclose(fptr);

    wait(NULL);
    wait(NULL);
}
