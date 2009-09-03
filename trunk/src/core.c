#include "../inc/typedef.h"
#include "../inc/core.h"
#include <pthread.h>
#include <stdio.h>

#define CUADRAS 6
#define XDIM 3*CUADRAS
#define YDIM 3*CUADRAS
#define TRUE 1
#define FALSE 0

boolean tiles[YDIM][XDIM];
point_t buses[XDIM*YDIM];
int next_id=0;
pthread_mutex_t next_id_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tiles_mutex[YDIM][XDIM] = PTHREAD_MUTEX_INITIALIZER;

int
main(void) {
  
}

int
init(void) {
    
    int i,j;
    
    /* LLeno la matriz de la ciudad, donde hay una manzana pongo TRUE, asi siempre
    esta ocupada y no se puede pasar por encima de una manzana. */
    for(i=0 ; i < YDIM ; i++){
        for(j=0 ; j < XDIM ; j++){
            if(((i%(CUADRAS/2) == 1) || (i%(CUADRAS/2) == 2)) 
                && ((j%(CUADRAS/2) == 1) || (j%(CUADRAS/2) == 2)))
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
	recieve
    }
}



int
insert_bus(int * id, point_t pos){
    
    int aux_id;
    
    if(!valid_pos(pos)){
        pthread_exit((void *)  NEW_POS_INVALID);
    }
    if(((pos.y%(CUADRAS/2) == 1) || (pos.y%(CUADRAS/2) == 2)) 
      && ((pos.x%(CUADRAS/2) == 1) || (pos.x%(CUADRAS/2) == 2))){
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
//     pthread_mutex_unlock(&instert_bus_mutex);
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
    if(tile[new_pos.y][new_pos.x]){
        printf("No se puede avanzar, nueva posicion esta ocupada.\n");
        return NEW_POS_ALREADY_OCCUPIED;
    }
    /* aca habria que chequear que esto sea atomico */
//     pthread_mutex_lock(&tiles_mutex[actual_pos.y][actual_pos.x]);
    bus[id] = new_pos;
    tiles[actual_pos.y][actual_pos.x] = FALSE;
    tiles[new_pos.y][new_pos.x] = TRUE;
//     pthread_mutex_unlock(&tiles_mutex[actual_pos.y][actual_pos.x]);
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
            
            
         