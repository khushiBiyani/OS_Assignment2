#include <stdio.h>
int main()
{
    for (int i=0;i<10;i++)
    {
        printf("This is process 2\n");
        sleep(1);
    }
    return 0;
}