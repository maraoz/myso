
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int wait_children(void) {
    int pid,status;
    while (1)
        {
            pid = waitpid(WAIT_ANY, &status, WNOHANG);
            if (pid > 0)
                break;
        }
    return pid;
}

int
main (void)
{
    FILE * fout = fopen("zergling.c", "w");
    char buf[80] = "#include <stdio.h>\nint main(void)\n{\n    printf(\"hello world!\\n\");\n}\n";
    fprintf(fout ,"%s", buf);
    fclose(fout);
    int pid;
    if ((pid = fork()) == 0) {
        /* slave gcc process */
        char *cmd[] = { "gcc", "zergling.c", "-ozergling", (char *)0 };
        execv("/usr/bin/gcc", cmd);
    } else {
        /* master process */
        pid = wait_children();
        printf("master waited for slave %d\n", pid);
        if ((pid = fork()) == 0) {
            /* slave zergling process */
            char *cmd[] = { "zergling", "", (char *)0 };
            execv("./zergling", cmd);
        } else {
            /* master process */
            pid = wait_children();
            printf("master finished and tired!!\n");
        }
    }


}