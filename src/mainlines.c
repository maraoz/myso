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

int
main(void){

    int * buses_times;
    pthread_t * buses_threads;
    int aux_pthread_creation;
    pthread_attr_t attr;
    int index = 0;
    int aux = 0;
    int tmp_qty_buses;

    m_init_line();
    my_pid = getpid();
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    buses.path_length = get_path(3, &buses.path);
    qty_buses = get_qty_buses(3);
    buses_times = get_times(3);
    
    
/*    
    
    buses.path_length = 12;
    buses.path = malloc(12*sizeof(point_t));
    buses.path[0].x = 0;
    buses.path[0].y = 0;
    buses.path[1].x = 0;
    buses.path[1].y = 1;
    buses.path[2].x = 0;
    buses.path[2].y = 2;
    buses.path[3].x = 0;
    buses.path[3].y = 3;
    buses.path[4].x = 1;
    buses.path[4].y = 3;
    buses.path[5].x = 2;
    buses.path[5].y = 3;
    buses.path[6].x = 3;
    buses.path[6].y = 3;
    buses.path[7].x = 3;
    buses.path[7].y = 2;
    buses.path[8].x = 3;
    buses.path[8].y = 1;
    buses.path[9].x = 3;
    buses.path[9].y = 0;
    buses.path[10].x = 2;
    buses.path[10].y = 0;
    buses.path[11].x = 1;
    buses.path[11].y = 0;

    qty_buses = 3;

    buses_times = malloc(3*sizeof(int));
    buses_times[0] = 0;
    buses_times[1] = 10;
    buses_times[2] = 15;
    
*/
    
    
    
    movements = calloc(sizeof(int),qty_buses);
    if(movements == NULL){
    return 1;
    }
    buses_threads = malloc(sizeof(pthread_t) * qty_buses);
    if(buses_threads == NULL){
        printf("No hay suficiente memoria \n");
        return 1;
    }
    buses.stops_length = get_stops(0, &buses.stops);
    
    
/*    
    
    
    buses.stops_length = 2;
    buses.stops = malloc(2*sizeof(point_t));
    buses.stops[0].x = 0;
    buses.stops[0].y = 3;
    buses.stops[1].x = 3;
    buses.stops[1].y = 0;
    
*/    
    
    
    aux = buses_times[0];
    tmp_qty_buses = qty_buses;
    while(qty_buses > 0) {
        usleep(aux);
        aux_pthread_creation = pthread_create(&buses_threads[index], &attr, (void*)(new_bus), (void *)index);
        if(!aux_pthread_creation){
	    while(tmp_qty_buses == qty_buses);
	    tmp_qty_buses--;
	    index++;
	    if(qty_buses > 0){
		aux = buses_times[index] - buses_times[index-1];
	    }
        } else {
            printf("No se puedo crear el colectivo \n");
            aux = 0;
        }
    }
    m_init_line();
    openChannel(1);
    while(sim_on){
        receive();
    }

    pthread_attr_destroy(&attr);
    free(movements);
}
