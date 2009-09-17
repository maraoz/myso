#include "../inc/typedef.h"
#include "../inc/core.h"
#include "../inc/draw.h"
#include "../inc/util.h"
#include "../inc/files.h"
#include "../inc/marshall.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>


extern WINDOW *log_win;

boolean tiles[XDIM][YDIM];
point_t buses[XDIM*YDIM][XDIM*YDIM];


semaphore semps[(CUADRAS+1)*(CUADRAS+1)-4];
int semps_hash[(CUADRAS+1)][(CUADRAS+1)];
pthread_mutex_t semaphore_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t map_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t citizen_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t citizen_cond = PTHREAD_COND_INITIALIZER;
person_t passenger;
extern sim_on;
Tfiles files;

int
init(void) {
    
    int i,j;
    point_t aux;
    int index = 0;
    /* LLeno la matriz de la ciudad, donde hay una manzana pongo TRUE, asi siempre
    esta ocupada y no se puede pasar por encima de una manzana. */
    for(i=0 ; i < YDIM ; i++){
        for(j=0 ; j < XDIM ; j++){
            aux.x = j;
            aux.y = i;
            if(hasSemaphore(aux)) {
                /*Seteo en Vertical Red el estado inicial de todos los semaforos*/
                semps[index].state = V_RED;
                /* Agrego en la matrix semps_hash el indice en el vecto 
                 * semps de dicho semaforo */
                semps_hash[aux.y][aux.x] = index; 
                semps[index++].pos = aux;
                tiles[i][j] = FALSE;
            } else if(((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) 
                && ((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2))) {
                tiles[i][j] = TRUE;
            } else {
                tiles[i][j] = FALSE;
            }
        }
    }
    m_init_core();
    return 0;
}

    


void *
core_listen(int index) {
    while(sim_on){
        receive(files.buffer[index]); /* En files.buffer tengo todos las session_t */
    }
    pthread_exit(0);
}


void *
keyboard_listen(){
    int ch,i;
    while((ch = getch()) != 'q'); /* Leo del teclado hasta leer una 'q' */
    for(i=0; i<files.qty; i++)
        delete_line(files.buffer[i],i); /* Envio señales para matar todas las lineas */
    sim_on = FALSE; /* Seteo la variable de simulacion activa en FALSE */
    pthread_exit(0);
}


void *
pax_creation() {
    int random_line;
    sleep(5); /* Espero 5 segundos para no empezar inmediatamente a crear pasajeros */
    while(sim_on){
        sleep(3); /*Tiempo entre creacion de pasajeros seteado en 3 segundos */
        random_line = rand()%files.qty; /* Elijo una linea al azar entre todas las que tengo disponible */
        pthread_mutex_lock(&citizen_mutex); 
        get_random_stops(files.buffer[random_line], random_line); 
        /* Envio mensaje para que me envien paradas y las espero */
        pthread_cond_wait(&citizen_cond,&citizen_mutex);
        if(random_line != passenger.line){
            /* Si el numero de parada elegido al azar no coincide con el seteado en el pasajero
             * hay un error */
            wprintw(log_win,"Paradas no corresponden a la linea pedida\n");
        } else {
            /* Inserto pasajero en su linea correspondiente */
            insert_pax_to_line(files.buffer[passenger.line], passenger.line, passenger.up, passenger.down);
            wprintw(log_win,"Nuevo pasajero para la linea %d\n",passenger.line+1);
            wprintw(log_win,"Va de la parada (%d,%d) a la parada (%d,%d)\n" ,passenger.up.x,passenger.up.y,passenger.down.x,passenger.down.y);
        }
        pthread_mutex_unlock(&citizen_mutex);
        
    }
    pthread_exit(0);
}



int
insert_bus(int idl, int idb, point_t pos){
    
    /* Reviso que sea una linea valida */
    if(idl >= XDIM*YDIM || idl < 0) {
        wprintw(log_win,"Linea no soportada por el sistema.\n");
        return FD_TOO_LARGE;
    }
    
    /* Reviso que sea un colectivo valido */
    if(idb >= XDIM*YDIM || idb < 0) {
        wprintw(log_win,"Colectivo no soportado por el sistema.\n");
        return ID_TOO_LARGE;
    }
    
    /* Reviso que la posicion sea valida */
    if(!valid_pos(pos)){
        return NEW_POS_INVALID;
    }
    
    /*Reviso que no caiga dentro de una manzana */
    if(((pos.y%(TILES_CUADRAS+1) == 1) || (pos.y%(TILES_CUADRAS+1) == 2)) 
      && ((pos.x%(TILES_CUADRAS+1) == 1) || (pos.x%(TILES_CUADRAS+1) == 2))){
        return BLOCKED_SLOT;
    }

    /*Reviso que el lugar nuevo este vacio */
    if(tiles[pos.y][pos.x]){
        return BUS_ALREADY_IN_SLOT;
    }

    /* Si llegue hasta acá, inserto el colectivo */
    pthread_mutex_lock(&map_mutex);
    tiles[pos.y][pos.x] = TRUE;
    buses[idl][idb] = pos;
    pthread_mutex_unlock(&map_mutex);
    insert_bus_ack(files.buffer[idl],idl,idb);

    return idb;
}


int
move_bus(int idl, int idb, point_t new_pos){
    /* Guardo la posicion actual del colectivo en una variable local */
    point_t actual_pos = buses[idl][idb];
    int aux; 
    
    /* Reviso que sea una linea valida */
    if(idl >= XDIM*YDIM || idl < 0) {
	    wprintw(log_win,"Linea no soportada por el sistema.\n");
        return FD_TOO_LARGE;
    }
    
    /* Reviso que sea un colectivo valido */
    if(idb >= XDIM*YDIM || idb < 0) {
	    wprintw(log_win,"Colectivo no soportado por el sistema.\n");
        return ID_TOO_LARGE;
    }  
    
    /* Reviso que new_pos sea una posicion valida */
    if(!valid_pos(new_pos)){
        return NEW_POS_INVALID;
    }
    
    /* Reviso que el colectivo no se quiera mover mas de 1 lugar por turno */
    if(dist(new_pos, actual_pos) > 1){
        return NEW_POS_FAR_AWAY;
    }
  
    /* Reviso que los semaforos esten en verde en la direccion que me quiero mover */
    if(hasSemaphore(new_pos) && isVRedHGreen(semps[(semps_hash[new_pos.y][new_pos.x])]) 
	&& actual_pos.x == new_pos.x) {
        return RED_LIGHT_ON;
    }
    if(hasSemaphore(new_pos) && !isVRedHGreen(semps[(semps_hash[new_pos.y][new_pos.x])]) 
    && actual_pos.y == new_pos.y) {
        return RED_LIGHT_ON;
    }
    
    if(tiles[new_pos.y][new_pos.x] == TRUE){
        /*Reviso que el lugar nuevo este vacio */
        return NEW_POS_ALREADY_OCCUPIED;
    }

    /* Reviso que ningun colectivo este moviendose en contramano */
    if((aux=(new_pos.x - actual_pos.x)) != 0) {
        if(new_pos.y%6==0  && aux != 1 && new_pos.y != YDIM-1) {
            wprintw(log_win,"CONTRAMANO.\n");
            return WRONG_WAY;
        }
        if(((new_pos.y%3==0 && new_pos.y%6!=0)|| new_pos.y==YDIM-1) && aux != -1  && new_pos.y != 0) {
            wprintw(log_win,"CONTRAMANO.\n");
            return WRONG_WAY;
        }
    } else if((aux=(new_pos.y - actual_pos.y)) != 0) {
        if(new_pos.x%6==0 && new_pos.x != XDIM-1 && aux != -1) {
            wprintw(log_win,"CONTRAMANO.\n");
            return WRONG_WAY;
        }
        if(((new_pos.x%3==0 && new_pos.x%6!=0) || new_pos.x == XDIM-1) && aux != 1 && new_pos.x != 0) {
            wprintw(log_win,"CONTRAMANO.\n");
            return WRONG_WAY;
        }
    }

    /* Si llegue hasta acá, muevo el colectivo */
    buses[idl][idb] = new_pos;
    pthread_mutex_lock(&map_mutex);
    tiles[actual_pos.y][actual_pos.x] = FALSE;
    tiles[new_pos.y][new_pos.x] = TRUE;
    pthread_mutex_unlock(&map_mutex);

    move_request_ack(files.buffer[idl],idl,idb);
    
    return idb;
}

int
valid_pos(point_t pos){
    
    if(pos.x >= XDIM || pos.x < 0 || pos.y >= YDIM || pos.y < 0){
            wprintw(log_win,"pos.x = %d, pos.y = %d\n", pos.x, pos.y);
            wprintw(log_win,"Posicion fuera las dimensiones de la ciudad.\n");
            return FALSE;
    }
    return TRUE;
}

int
hasSemaphore(point_t pos){
    if( !((pos.y == 0 || pos.y == CUADRAS*(TILES_CUADRAS+1))
	&& (pos.x == 0 || pos.x == CUADRAS*(TILES_CUADRAS+1)))
	&& ((pos.y % (TILES_CUADRAS+1) == 0) && (pos.x % (TILES_CUADRAS+1) == 0)))
	    return 1;
    return 0;
}

int
isVRedHGreen(semaphore s){
    if( s.state == V_RED ){
        return TRUE;
    }
    return FALSE;
}

void
switch_semaphore(semaphore * sem){
    /* Cambio el estado del semaforo */
    sem->state = !sem->state;
}

int
set_new_pax(int idl, point_t stop_up, point_t stop_down){
    
    pthread_mutex_lock(&citizen_mutex);
    
    /* Creo el nuevo pasajero */
    passenger.line = idl;
    passenger.up = stop_up;
    passenger.down = stop_down;
    
    pthread_cond_signal(&citizen_cond);
    pthread_mutex_unlock(&citizen_mutex);
    return 0;
}

int
pax_get_of_bus(int idl, int idb, point_t stop){
    wprintw(log_win,"Pasajero de linea %d, colectivo %d, bajandose en (%d,%d)\n", idl+1, idb+1, stop.x, stop.y);
    return 0;
}

int
pax_get_on_bus(int idl, int idb, point_t stop){
    wprintw(log_win,"Pasajero de linea %d, colectivo %d, subiendose en (%d,%d)\n", idl+1, idb+1, stop.x, stop.y);
    return 0;
}
         
