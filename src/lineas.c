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
int ** pax;

pthread_mutex_t pax_mutex = PTHREAD_MUTEX_INITIALIZER;


void 
new_pax(int fd, int id, point_t start, point_t stop){
    int i,j=-1,k=-1;
    for(i=0;i<buses.stops_length;i++){
        if(start.x != buses.stops[i].x && start.y != buses.stops[i].y){
            j = i;
        }
        if((stop.x != buses.stops[i].x && stop.y != buses.stops[i].y)){
            k = i;
        }
    }
    if(k==-1 || j==-1){
        printf("No existe dicha parada\n");
        return;
    }
    pthread_mutex_lock(&pax_mutex);
    if((pax[j][0]+1) % 10 == 0){
        pax[j] = realloc(pax[j],(pax[j][0]+1)+10);   
        if(pax[j] == NULL)
            printf("No hay suficiente memoria\n");
    }
    pax[j][pax[j][0]++] = k;
    pthread_mutex_unlock(&pax_mutex);
}


void
calculate_stops(int fd){
    srand(time(NULL));
    int stop_up, stop_down;
    stop_up = rand()%buses.stops_length;
    stop_down = rand()%buses.stops_length;
    deliver_stops(session, line_id, stop_up, stop_down);
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
            printf("estoy en una parada con %d pasajeros\n",pax[j][0]);
            pthread_mutex_lock(&pax_mutex);
            while(pax[j][0] > 0){
                sleep(1);                
                pax_arriba[pax[j][pax[j][0]]]++;
                pax[j][0]--;
            }
            pthread_mutex_unlock(&pax_mutex);
            while(pax_arriba[j] > 0){
                sleep(1);
                pax_arriba[j]--;
                pax_downloaded(session, line_id, index, buses.stops[j]); 
            }
            j++;
            j = j%buses.stops_length;
        }
    }
}
