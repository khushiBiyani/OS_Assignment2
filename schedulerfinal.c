#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main()
{
    pid_t pro1_pid = -1, pro2_pid = -1, sch_pid = -1;
    sch_pid = getpid();
    pro1_pid = fork(); // returns 0 to child and child-pid to parent.
    if (pro1_pid == 0) // if child (pro1)
    {

        execl("./pro1", NULL);
    }

    // parent
    pro2_pid = fork();
    if (pro2_pid == 0) // if child (pro2)
    {
        execl("./pro2", NULL);
    }

    printf("%d %d \n", pro1_pid, pro2_pid);

    int quantum_number = 0;

    //pause both processes
    kill(pro1_pid,SIGSTOP); // we are NOT killing the processes.
    kill(pro2_pid,SIGSTOP);
    
    kill(pro1_pid,SIGCONT); 
    while (1)
    {
        printf("QUANTUM NUMBER: %d\n",quantum_number);
        if (quantum_number%2==0) //even implies pro1 should execute
        {
            kill(pro2_pid,SIGSTOP);   
            kill(pro1_pid,SIGCONT);
        }
        else
        {
            kill(pro1_pid,SIGSTOP);   
            kill(pro2_pid,SIGCONT);
        }
        quantum_number++;
        sleep(5);
    }
    wait(NULL);
    return 0;
}