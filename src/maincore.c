#include "../inc/typedef.h"
#include "../inc/core.h"
#include "../inc/draw.h"
#include "../inc/util.h"
#include "../inc/files.h"
#include "../inc/protocol.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>


extern boolean tiles[YDIM][XDIM];
extern point_t buses[XDIM*YDIM][XDIM*YDIM];

extern semaphore semps[(CUADRAS+1)*(CUADRAS+1)-4];
extern int semps_hash[(CUADRAS+1)][(CUADRAS+1)];
extern pthread_mutex_t semaphore_mutex;

extern pthread_mutex_t map_mutex;

extern person_t passenger;
extern pthread_mutex_t citizen_mutex;
extern pthread_cond_t citizen_cond;

extern WINDOW *log_win;

extern Tfiles files;

extern ipc_selection;

int sim_on = TRUE;

char * ipc_strings[] = {"SHARED MEMORY", "FIFO", "SOCKET", "MESSAGE QUEUE"};

int
main(int argc, char * argv[]) {

    if (argc == 2){
        char * option = argv[1];
        int int_opt = atoi(option);
        if (int_opt >= 0 && int_opt < 4 ) {
            ipc_selection = int_opt;
        } else {
            printf("C�digo de IPC inv�lido, se toma el default"
            "MESSAGE_QUEUE\n");
            ipc_selection = MESSAGE_QUEUE;
        }
    } else {
        ipc_selection = MESSAGE_QUEUE;
    }
    printf("Se seleccionó el IPC %s\n", ipc_strings[ipc_selection]);
    sleep(1);

    pthread_t * core_threads;
    pthread_attr_t attr;
    int aux_pthread_creation;
    int i;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    files.buffer = malloc(10*sizeof(int));
    files.qty = 0;
    if(files.buffer == NULL)
        return 1;

    init();

    openDir();

    while((files.buffer[files.qty] = openFiles()) != 0) {

    if(files.buffer[files.qty] != -1){
        pid_t pid;
        int aux;
        char * line_id;
        char * ipc_method;
        line_id = itoa(files.qty);
        ipc_method = itoa(ipc_selection);
        preparefd(files.buffer[files.qty]);
        closeFd(files.buffer[files.qty]);
        char *args[] = {"lineas", line_id, ipc_method ,(char *) 0 };
        pid = fork();
        switch(pid){
            case 0: aux = execv("../bin/lineas", args);
                if(aux == -1){
                    return -1;
                }
            case -1:
                    return 1;
            default:
                files.buffer[files.qty] = openChannel(pid);
                break;
        }
        files.qty++;
        if(files.qty%10 == 0){
            files.buffer = realloc(files.buffer,(files.qty+10)*sizeof(int));
	    // TODO: CHEQUEAR POR NULL
            }
        }
    }
    closeDir();

    core_threads = malloc((files.qty+3) * sizeof(pthread_t));
    if(core_threads == NULL){
	printf("No hay suficiente memoria\n");
	return 1;
    }

    aux_pthread_creation = pthread_create(&core_threads[0], &attr, (void*)(draw), NULL);
    if(aux_pthread_creation){
       wprintw(log_win, "+ERROR: No se pudo crear el thread pedido.\n");
    }
    aux_pthread_creation = pthread_create(&core_threads[1], &attr, (void*)(pax_creation), NULL);
    if(aux_pthread_creation){
       wprintw(log_win, "+ERROR: No se pudo crear el thread pedido.\n");
    }
    aux_pthread_creation = pthread_create(&core_threads[2], &attr, (void*)(keyboard_listen), NULL);
    if(aux_pthread_creation){
       wprintw(log_win, "+ERROR: No se pudo crear el thread pedido.\n");
    }

    for(i=0;i<files.qty;i++){
        aux_pthread_creation = pthread_create(&core_threads[i+3], &attr, (void*)(core_listen), (void*)i);
        if(aux_pthread_creation){
            wprintw(log_win, "+ERROR: No se pudo crear el thread pedido.\n");
        }   
    }
    pthread_attr_destroy(&attr);

    while(sim_on){
        int i;
        sleep(10);
        pthread_mutex_lock(&semaphore_mutex);
        for(i = 0; i<(CUADRAS+1)*(CUADRAS+1) - 4 ; i++)
            switch_semaphore(&semps[i]);
        pthread_mutex_unlock(&semaphore_mutex);
    }
    pthread_mutex_destroy(&semaphore_mutex);
    pthread_mutex_destroy(&map_mutex);
    pthread_mutex_destroy(&citizen_mutex);
    pthread_cond_destroy(&citizen_cond);
    closeFd(3);
    for (i = 0; i<files.qty; i++)
        closeChannel(files.buffer[i]);
    exit(0);
}
