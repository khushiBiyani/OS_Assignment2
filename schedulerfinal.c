#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
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

    pid_t pro1_pid = -1, pro2_pid = -1, sch_pid = -1;
    sch_pid = getpid();
    pro1_pid = fork();    // returns 0 to child and child-pid to parent.
    if (pro1_pid == 0)    // if child (pro1)
    {
        execlp("./p1", "./p1", I, J, K, inputFileOne, inputFileTwo, outputFile, MAXTHREADS, NULL);
    }
    kill(pro1_pid, SIGSTOP);    // we are NOT killing the processes.

    pro2_pid = fork();
    if (pro2_pid == 0)    // if child (pro2)
    {
        execlp("./p2", "./p2", MAXTHREADS, NULL);
    }
    kill(pro2_pid, SIGSTOP);

    int turn = 0;

    int pro1alive = 141221;
    int pro2alive = 141221;

    while (1) {
        waitpid(pro1_pid, &pro1alive, WNOHANG);
        waitpid(pro2_pid, &pro2alive, WNOHANG);
        if (pro1alive == 141221 && pro2alive == 141221) {
            if (turn % 2 == 0)    // even implies pro1 should execute
            {
                kill(pro2_pid, SIGSTOP);
                kill(pro1_pid, SIGCONT);
            } else {
                kill(pro1_pid, SIGSTOP);
                kill(pro2_pid, SIGCONT);
            }
        } else if (pro1alive == 141221 && pro2alive != 141221) {
            kill(pro1_pid, SIGCONT);
        } else if (pro1alive != 141221 && pro2alive == 141221) {
            kill(pro2_pid, SIGCONT);
        } else {
            break;
        }
        turn++;
        usleep(5000);
    }

    wait(NULL);
    wait(NULL);
}
