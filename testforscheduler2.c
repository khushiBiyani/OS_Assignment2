#include <stdio.h>
#include <unistd.h>

int main()
{
    for (int i=0;i<10;i++)
    {
        printf("This is process 2\n");
    }
    return 0;
}