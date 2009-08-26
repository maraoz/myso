
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main (void)
{
    FILE * fin = fopen("hello.in", "r");
    char buf[80];
    int pid;
    while (fgets(buf, 80, fin)) {
        if ((pid = fork()) == 0) {
            /* slave process */
            pid = getpid();
            int n = atoi(buf);
            n *= n;
            printf("%d slave calculated: %d\n", pid, n);
            break;
        } else {
            /* master process */
            printf("master created slave %d\n", pid);
        }
    }

}