#include "../inc/typedef.h"
#include "../inc/lineas.h"
#include "../inc/files.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

extern buses_line buses;
extern pid_t my_pid;
int sim_on = TRUE;
extern int * movements;
extern int qty_buses;
extern int tmp_qty_buses;
extern session_t session;
extern int line_id;
extern int * insertion_ack;
extern int ** pax;
extern pthread_mutex_t pax_mutex;
// extern WINDOW *log_win;
extern int ipc_selection;

int
main(int argc, char * argv[] ){

    ipc_selection = atoi(argv[2]);
    m_init_line();

    session = openChannel(1);

    line_id = atoi(argv[1]);

    int * buses_times;
    pthread_t * buses_threads;
    pthread_t listener_thread;
    int aux_pthread_creation;
    pthread_attr_t attr;
    int index = 0;
    int aux = 0;
    int i;
    int tmp_qty_buses;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    buses.path_length = get_path(3, &buses.path);
    qty_buses = get_qty_buses(3);
    buses_times = get_times(3);

    
    movements = calloc(sizeof(int),qty_buses);
    if(movements == NULL){
	printf("+ERROR: No hay suficiente memoria\n");
	return 1;
    }
    for(i =0 ; i<qty_buses; i++){
        movements[i] = -1;
    }
     
    buses_threads = malloc(sizeof(pthread_t) * qty_buses);
    
    if(buses_threads == NULL){
        printf("+ERROR: No hay suficiente memoria \n");
        return 1;
    }
   buses.stops_length = get_stops(3, &buses.stops);
    
    closeFd(3);
    pax = malloc(buses.stops_length*sizeof(int*));
    if(pax == NULL){
	printf("+ERROR: No hay suficiente memoria\n");
    }
    for(i = 0 ; i<buses.stops_length; i++){
	pax[i] = malloc(11*sizeof(int));
	if(pax[i] == NULL){
	    printf("+ERROR: No hay suficiente memoria\n");
	}
    }

    aux = buses_times[0];
    
    aux_pthread_creation = pthread_create(&listener_thread, &attr, (void*)(line_listener), (void *)0);
    while(qty_buses > 0) {
        sleep(aux);
        aux_pthread_creation = pthread_create(&buses_threads[index], &attr, (void*)(new_bus), (void *)index);
	    index++;
        qty_buses--;
	    if(qty_buses > 0){
            aux = buses_times[index] - buses_times[index-1];
	    } else {
// 		printf("+ERROR: No se pudo crear el colectivo \n");
            aux = 0;
        }
    }
   
    pthread_exit(0);
}

void * 
line_listener(){
    while(sim_on){
        receive(session);
    }
    pthread_exit(0);
}
