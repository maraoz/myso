#include "../inc/typedef.h"
#include "../inc/lineas.h"
#include "../inc/files.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


typedef struct {
    point_t * path;
    point_t * stops;
    int path_length;
    int stops_length;
} buses_line;

buses_line buses;
pid_t my_pid;
extern int sim_on;
int * movements;
int qty_buses;

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
    
    aux = buses_times[0];
    tmp_qty_buses = qty_buses;
    while(qty_buses > 0) {
        usleep(aux);
        aux_pthread_creation = pthread_create(&buses_threads[index], &attr, (void*)(new_bus), (void *)index);
        if(!aux_pthread_creation){
	    while(tmp_qty_buses == qty_buses);
            tmp_qty_buses--;
            index++;
            aux = buses_times[index] - buses_times[index-1];
        } else {
            printf("No se puedo crear el colectivo \n");
            aux = 0;
        }
    }
    m_init_line();
    openChannel(1);
    while(sim_on){
	receive_lines();
    }

    pthread_attr_destroy(&attr);
    free(movements);
}

void
move_ack(int fd, int id){
    movements[id]++;
}

void
insert_ack(int id){
    qty_buses--;
}

void *
new_bus(int index) {
    int my_index = index;
    int i=0,j=0;
    insert_request(my_pid, index, buses.path[i]);
    i++;j++;
    while(sim_on){
        usleep(500);
	i = i%buses.path_length;
        move_request(my_pid, index, buses.path[i]);
	if(movements[my_index] > i) {
	    if(buses.path[i].x == buses.stops[j].x && buses.path[i].y == buses.stops[j].y ) {
		j = j%buses.stops_length;
		usleep(500);
		j++;
	    }
	    i++;
	}
    }
}
