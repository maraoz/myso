#include <pthread.h>
#include "../inc/typedef.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    point_t * path;
    point_t * stops;
    int path_length;
    int stops_length;
} buses_line;

bus_line buses;

int
main(void){

    int qty_buses;
    pthread_t * buses_threads;
    int aux_pthread_creation;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    buses.path_length = get_path(0, &buses.path);
    qty_buses = get_qty_buses(0);
    buses_threads = malloc(sizeof(pthread_t) * qty_buses);
    if(buses_threads == NULL){
	printf("No hay suficiente memoria \n");
	return 1;
    }
    buses.stops_length = get_stops(0, &buses.stops);

    while(qty_buses > 0) { 
	aux_pthread_creation = pthread_create(&threads[qty_buses], attr, new_bus, (void *)t);
	if(!aux_pthread_creation){
	    qty_buses--;
	} else {
	    printf("No se puedo crear el colectivo \n");
	}
    }
    
}

void *
new_bus() {
    insert_request(int * id, point_t pos);
}
