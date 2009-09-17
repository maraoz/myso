#include "../inc/typedef.h"
#include "../inc/lineas.h"
#include "../inc/files.h"
#include "../inc/core.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

/* Variables globales */
buses_line buses;
session_t session;
extern int sim_on;
int * movements;
int qty_buses;
int tmp_qty_buses;
int line_id;
int * insertion_ack;
int ** pax;
pthread_mutex_t pax_mutex = PTHREAD_MUTEX_INITIALIZER;

extern WINDOW *log_win;

void 
end_line(int line_id) {
    sim_on = FALSE; /* Setea la variable de simulacion activa en FALSE */
    closeChannel(session); /* Cierra conexion */
    pthread_mutex_destroy(&pax_mutex);
    free(movements);
    exit(0);
}

void 
new_pax(int idl, point_t start, point_t stop){
    int i,j=-1,k=-1;
    /* A partir de dos puntos, obtengo los indices en el vector de paradas */
    for(i=0;i<buses.stops_length;i++){
        if(start.x == buses.stops[i].x && start.y == buses.stops[i].y){
            j = i;
        }
        if((stop.x == buses.stops[i].x && stop.y == buses.stops[i].y)){
            k = i;
        }
    }
    /* Si alguno de los dos indices es -1, quiere decir que esa parada
     * no pertenece a la linea */
    if(k==-1 || j==-1){
        printf("+ERROR: No existe dicha parada\n");
        return;
    }
    pthread_mutex_lock(&pax_mutex);
    /* En pax[j][0] tengo la cantidad de personas esperando en la parada j */
    if(pax[j][0] != 0 && (pax[j][0]) % 10 == 0){
        pax[j] = realloc(pax[j],((pax[j][0]+1)+10)*sizeof(int));   
        if(pax[j] == NULL){
            printf("+ERROR: No hay suficiente memoria\n");
	    return;
	}
    }
    /* En la primera posicion del vector de pasajeros que esta vacia 
     * guardo la parada donde se tiene que bajar */
    pax[j][pax[j][0]++] = k; 
    pthread_mutex_unlock(&pax_mutex);
}


void
calculate_stops(int fd){
    srand(time(NULL));
    int stop_up, stop_down;
    stop_up = rand()%buses.stops_length; /* Calculo parada random */
    stop_down = rand()%buses.stops_length; /* Calculo parada random */
    /* Envio las paradas al core */
    deliver_stops(session, line_id, buses.stops[stop_up], buses.stops[stop_down]);
}
    
    

void
move_ack(int fd, int id){
    if(movements[id] == -1){
         printf("+ERROR: no existe el colectivo\n");
        return;
    }
    /* Incremento la posicion del colectivo que obtuvo permiso para moverse */
    movements[id]++; 
    movements[id]%=buses.path_length;
}

void
insert_ack(int fd, int id){
    /* Reviso que no este insertado todavia */
    if(movements[id] == -1){
        movements[id] = 1;
    }
}


void *
new_bus(int index) {
    int my_index = index;
    int i=0,j=0;
    int * pax_arriba;

    pax_arriba = calloc(buses.stops_length, sizeof(int));

    /* Intento insertar, hasta que pude insertar el colectivo */
    while(movements[my_index] == -1){
        insert_request(session, line_id, index, buses.path[0]);
        usleep(100000); /* Tiempo de espera entre pedidos de insercion */
    }
    while(sim_on){
        sleep(2); /* Tiempo de espera entre pedidos de movimiento */
        move_request(session, line_id, index, buses.path[movements[my_index]]);
	    /* Si la posicion en la que estoy es la siguiente parada, veo si hay gente para subir o bajar */
	    if(buses.path[movements[my_index]].x == buses.stops[j].x && buses.path[movements[my_index]].y == buses.stops[j].y ) {
            pthread_mutex_lock(&pax_mutex);
	    /* Hasta que la cantidad de pasajeros a subir sea 0, voy subiendo pasajeros de a 1*/
            while(pax[j][0] > 0){
                sleep(1);                
                pax_arriba[pax[j][pax[j][0]]]++;
                pax[j][0]--;
		/* Aviso al core que subio un pasajero */
		pax_uploaded(session, line_id, index, buses.stops[j]); 
            }
	    pax[j] = realloc(pax[j],11*sizeof(int));
	    if(pax[j] == NULL){
		printf("+ERROR: No hay suficiente memoria\n");
		return NULL;
	    }
	    pax[j][0] = 0;
            pthread_mutex_unlock(&pax_mutex);
	    /* Hasta que la cantidad de pasajeros a bajar sea 0, voy bajando pasajeros de a 1 */
            while(pax_arriba[j] > 0){
                sleep(1);
                pax_arriba[j]--;
		 /* Aviso al core que bajo un pasajero */
                pax_downloaded(session, line_id, index, buses.stops[j]);
            }
            j++; /* Aumento el indice para indicar que ya estuve en este parada */
            j = j%buses.stops_length;
        }
    }
   pthread_exit(0);
}
