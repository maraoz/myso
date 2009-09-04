#include "../inc/typedef.h"
#include "../inc/lineas.h"
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

int
main(void){

    int qty_buses;
    int * buses_times;
    pthread_t * buses_threads;
    int aux_pthread_creation;
    pthread_attr_t attr;
    int index = 0;
    int aux = 0;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    buses.path_length = get_path(0, &buses.path);
    qty_buses = get_qty_buses(0);
    buses_times = get_times(0);
    
    buses_threads = malloc(sizeof(pthread_t) * qty_buses);
    if(buses_threads == NULL){
        printf("No hay suficiente memoria \n");
        return 1;
    }
    buses.stops_length = get_stops(0, &buses.stops);
    
    aux = buses_times[0];
    while(qty_buses > 0) {
        usleep(aux);
        aux_pthread_creation = pthread_create(&buses_threads[index], &attr, (void*)(new_bus), (void *)index);
        if(!aux_pthread_creation){
            qty_buses--;
            index++;
            aux = buses_times[index] - buses_times[index-1];
        } else {
            printf("No se puedo crear el colectivo \n");
            aux = 0;
        }
    }
    pthread_attr_destroy(&attr);

}

void *
new_bus(int index) {
    init(host);
    int my_index = index;
    int i=0,j=0;
    insert_request(my_pid, index, buses.path[i]);
    i++;j++;
    while(sim_on){
        usleep(500);
        if(!(buses.path[i] == buses.stops[j])){
            i = i%buses.path;
            move_request(my_pid, index, buses.path[i]);
            i++;
        }
        else {
            j = j%buses.path;
            usleep(500);
            j++;
        }
    }
}
