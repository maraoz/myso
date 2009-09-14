#include "../inc/typedef.h"
#include "../inc/lineas.h"
#include "../inc/files.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

extern buses_line buses;
extern pid_t my_pid;
extern int sim_on;
extern int * movements;
extern int qty_buses;
extern int tmp_qty_buses;
extern session_t session;
extern int line_id;
extern int * insertion_ack;
extern int * pax;
extern pthread_mutex_t pax_mutex;

int
main(int argc, char * argv[] ){

    
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
    
    

//    buses.path_length = 12;
//     printf("cantidad de recorridos = %d\n", buses.path_length);
//     buses.path = malloc(12*sizeof(point_t));
//     buses.path[0].x = 0;
//     buses.path[0].y = 0;
//     buses.path[1].x = 1;
//     buses.path[1].y = 0;
//     buses.path[2].x = 2;
//     buses.path[2].y = 0;
//     buses.path[3].x = 3;
//     buses.path[3].y = 0;
//     buses.path[4].x = 3;
//     buses.path[4].y = 1;
//     buses.path[5].x = 3;
//     buses.path[5].y = 2;
//     buses.path[6].x = 3;
//     buses.path[6].y = 3;
//     buses.path[7].x = 2;
//     buses.path[7].y = 3;
//     buses.path[8].x = 1;
//     buses.path[8].y = 3;
//     buses.path[9].x = 0;
//     buses.path[9].y = 3;
//     buses.path[10].x = 0;
//     buses.path[10].y = 2;
//     buses.path[11].x = 0;
//     buses.path[11].y = 1;
//     for( mycont = 0 ; mycont < buses.path_length ; mycont++ )
// 	printf("recorrido %d = %d %d\n", mycont, buses.path[mycont].x, buses.path[mycont].y);

//     qty_buses = 3;
//     printf("cantidad de colectivos = %d\n", qty_buses);

//     buses_times = malloc(3*sizeof(int));
//     buses_times[0] = 1;
//     buses_times[1] = 4;
//     buses_times[2] = 7;
//     for( mycont = 0 ; buses_times[mycont] ; mycont++ )
// 	printf("tiempos = %d\n", buses_times[mycont]);
    
    
    
    movements = calloc(sizeof(int),qty_buses);
    for(i =0 ; i<qty_buses; i++){
        movements[i] = -1;
    }
    if(movements == NULL){
        return 1;
    }
    buses_threads = malloc(sizeof(pthread_t) * qty_buses);
    
    if(buses_threads == NULL){
        printf("No hay suficiente memoria \n");
        return 1;
    }
   buses.stops_length = get_stops(3, &buses.stops);
    
   
    
    
//     buses.stops_length = 2;
//     printf("cantidad de paradas = %d\n", buses.stops_length);
// 
//     buses.stops = malloc(2*sizeof(point_t));
//     buses.stops[0].x = 2;
//     buses.stops[0].y = 0;
//     buses.stops[1].x = 3;
//     buses.stops[1].y = 1;
//     for( mycont = 0 ; mycont < buses.stops_length ; mycont++ )
// 	printf("paradas %d = %d %d\n", mycont, buses.stops[mycont].x, buses.stops[mycont].y);

    pax = malloc(buses.stops_length, sizeof(int*));
    if(pax == NULL){
	printf("No hay suficiente memoria\n");
    }
    for(i = 0 ; i<buses.stops_length, i++){
	pax[i] = malloc(1*sizeof(int));
	if(pax[i] == NULL){
	    printf("No hay suficiente memoria\n");
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
            //printf("No se pudo crear el colectivo \n");
            aux = 0;
        }
    }
   
    while(sim_on);
    
    pthread_attr_destroy(&attr);
    free(movements);
}

void * 
line_listener(){
    while(sim_on){
        receive(session);
    }
}
