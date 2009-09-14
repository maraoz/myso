#include "../inc/typedef.h"
#include "../inc/lineas.h"
#include "../inc/files.h"
#include "../inc/core.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


buses_line buses;
session_t session;
extern int sim_on;
int * movements;
int qty_buses;
int tmp_qty_buses;
pid_t my_pid;
int line_id;
int * insertion_ack;
int * pax;
pthread_mutex_t pax_mutex = PTHREAD_MUTEX_INITIALIZER;;

void
new_pax(point_t stop){
    int i;
    for(i=0;i<buses.stops_length && stop.x != buses.stops[i].x && stop.y != buses.stops[i].y ;i++);
    if(i == buses.stops_length){
        printf("Esa parada no pertenece a esta linea\n");
    }
    pthread_mutex_lock(&pax_mutex);
    pax[i]++;   
    pthread_mutex_unlock(&pax_mutex);
}

void
move_ack(int fd, int id){
        if(DEBUG_MODE)
        printf("TRACE: ME MOVIIII WIII\n");
    if(movements[id] == -1){
         printf("no existe el colectivo\n");
        return;
    }
    movements[id]++;
    movements[id]%=buses.path_length;
}

void
insert_ack(int fd, int id){
    
    if(movements[id] == -1){
        if(DEBUG_MODE)
        printf("TRACE: ME INSERTARON WIII\n");
        movements[id] = 1;
    }
}

void *
new_bus(int index) {
    int my_index = index;
    int i=0,j=0;
    int * pax_arriba;

    pax_arriba = calloc(buses.stops_length, sizeof(int));

    while(movements[my_index] == -1){
        insert_request(session, line_id, index, buses.path[0]);
        usleep(100000);
    }
    if(DEBUG_MODE)
    printf("sali del while\n");
   while(sim_on){
        sleep(2);
        if(DEBUG_MODE)
        printf("intenando moverme\n");
        move_request(session, line_id, index, buses.path[movements[my_index]]);
	    if(buses.path[movements[my_index]].x == buses.stops[j].x && buses.path[movements[my_index]].y == buses.stops[j].y ) {
            pthread_mutex_lock(&pax_mutex);
            while(pax[j] > 0){
                pax[j]--;
                pax_arriba[j]++;
            }
            pthread_mutex_unlock(&pax_mutex);
            sleep(2);
            j++;
            j = j%buses.stops_length;
	    }
    }
}
