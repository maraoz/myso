#include "../inc/typedef.h"
#include "../inc/lineas.h"
#include "../inc/files.h"
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

/*    while(tmp_qty_buses == qty_buses)*/{
	insert_request(session, line_id, index, buses.path[i]);
	i++;j++;
    }
    
    while(sim_on){
        usleep(10000000);
	i = i%buses.path_length;
        move_request(session, line_id, index, buses.path[i]);
	if(movements[my_index] > i) {
	    if(buses.path[i].x == buses.stops[j].x && buses.path[i].y == buses.stops[j].y ) {
		j = j%buses.stops_length;
		usleep(10000);
		j++;
	    }
	    i++;
	}
    }
}
