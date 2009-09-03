#include "../inc/typedef.h"
#include "../inc/core.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


#define CUADRAS 6
#define TILES_CUADRAS 2
#define XDIM 3*CUADRAS
#define YDIM 3*CUADRAS
#define TRUE 1
#define FALSE 0
#define V_GREEN 1
#define V_RED	0
#define	H_GREEN V_RED
#define	V_GREEN H_RED

boolean tiles[YDIM][XDIM];
point_t buses[XDIM*YDIM];
semaphore semps[(CUADRAS+1)*(CUADRAS+1)-4];
int semps_hash[(CUADRAS+1)][(CUADRAS+1)];
int next_id=0;
pthread_mutex_t semaphore_mutex = PTHREAD_MUTEX_INITIALIZER;






int
main(void) {
 
    
    while(1){
	usleep(1000);
	pthread_mutex_lock(&semaphore_mutex);
	for(i = 0; i<(CUADRAS+1)*(CUADRAS+1) - 4 ; i++)
	    switch_semaphore(&semps[i]);
	pthread_mutex_unlock(&semaphore_mutex);

    }
    
}

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
		semps[index].state = V_RED;
		semps_hash[aux.y][aux.x] = index;
		semps[index++].pos = aux;
	    }
            else if(((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) 
                && ((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2)))
                tiles[i][j] = TRUE;
            else
                tiles[i][j] = FALSE;
        }
    }

    return 0;
}

void *
listen() {
    while(1){

	

    }
}



int
insert_bus(int * id, point_t pos){
    
    int aux_id;
    
    if(!valid_pos(pos)){
        pthread_exit((void *)  NEW_POS_INVALID);
    }
    if(((pos.y%(TILES_CUADRAS+1) == 1) || (pos.y%(TILES_CUADRAS+1) == 2)) 
      && ((pos.x%(TILES_CUADRAS+1) == 1) || (pos.x%(TILES_CUADRAS+1) == 2))){
        printf("No se puede insertar un colectivo en medio de una manzana.\n");
        return BLOCKED_SLOT;
    }

    if(tiles[pos.y][pos.x]){
        printf("Hay un bus en ese lugar, intentar luego.\n");
        return BUS_ALREADY_IN_SLOT;
    }
    
    tiles[pos.y][pos.x] = TRUE;
//     pthread_mutex_lock(&instert_bus_mutex);
    aux_id = next_id
    buses[next_id] = pos;
    (*id) = next_id;
    next_id++;
//    pthread_mutex_unlock(&instert_bus_mutex);
    return aux_id;
        
}


int
move_bus(int id, point_t new_pos){
    point_t actual_pos = buses[id];
    if(!valid_pos(new_pos)){
        return  NEW_POS_INVALID;
    }
    if(dist(new_pos, actual_pos) > 1){
        printf("No te podes mover mas de 1 lugar por turno.\n");
       return NEW_POS_FAR_AWAY;
    }
    pthread_mutex_lock(&semaphore_mutex);
    if(hassSemaphore(new_pos) && isVRedHGreen(semps[(semps_hash[pos.y][pos.x])]) 
	&& actual_pos.x == new_pos.x) {
	printf("No se puede avanzar, semaforo en rojo.\n");
        return RED_LIGHT_ON;
    }
    pthread_mutex_unlock(&semaphore_mutex);
    if(tile[new_pos.y][new_pos.x] == TRUE){
        printf("No se puede avanzar, nueva posicion esta ocupada.\n");
        return NEW_POS_ALREADY_OCCUPIED;
    }

	
    bus[id] = new_pos;
    tiles[actual_pos.y][actual_pos.x] = FALSE;
    tiles[new_pos.y][new_pos.x] = TRUE;
    return id;
}

int
valid_pos(point_t pos){
    
    if(pos.x >= XIMD || pos.x < 0 
        || pos.y >= YDIM || pos.y < 0){
            printf("Posicion fuera las dimensiones de la ciudad.\n");
            return FALSE;
    }
    return TRUE;
}

int
hassSemaphore(point_t pos){
    if( !((pos.y == 0 || pos.y == CUADRAS*TILES_CUADRAS+1) 
	&& (pos.X == 0 || pos.X == CUADRAS*TILES_CUADRAS+1))
	&& ((pos.y % TILES_CUADRAS == 0) && (pos.x % TILES_CUADRAS == 0)))
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
switch_semaphore(sempahore * sem){
    sem->state = !sem->state;
}

            
         