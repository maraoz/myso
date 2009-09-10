#include "../inc/typedef.h"
#include "../inc/core.h"
#include "../inc/draw.h"
#include "../inc/util.h"
#include "../inc/files.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>




boolean tiles[YDIM][XDIM];
point_t buses[XDIM*YDIM][XDIM*YDIM];


semaphore semps[(CUADRAS+1)*(CUADRAS+1)-4];
int semps_hash[(CUADRAS+1)][(CUADRAS+1)];
pthread_mutex_t semaphore_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t map_mutex = PTHREAD_MUTEX_INITIALIZER;
int sim_on = 1;


int session_line;

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
listen() {
    while(sim_on){
        receive(session_line);
    }
}



int
insert_bus(int fd, int id, point_t pos){
        
    if(fd >= XDIM*YDIM || fd < 0) {
        printf("Linea no soportada por el sistema.\n");
        return FD_TOO_LARGE;
    }
    
    if(id >= XDIM*YDIM || id < 0) {
        printf("Colectivo no soportado por el sistema.\n");
        return ID_TOO_LARGE;
    }
    
    if(!valid_pos(pos)){
        return NEW_POS_INVALID;
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
    
    pthread_mutex_lock(&map_mutex);
    
    tiles[pos.y][pos.x] = TRUE;
    buses[fd][id] = pos;
    pthread_mutex_unlock(&map_mutex);
    insert_bus_ack(session_line,fd,id);
    return id;
        
}


int
move_bus(int fd, int id, point_t new_pos){
    point_t actual_pos = buses[fd][id];
    int aux; 
    
    if(fd >= XDIM*YDIM || fd < 0) {
        printf("Linea no soportada por el sistema.\n");
        return FD_TOO_LARGE;
    }
    
    if(id >= XDIM*YDIM || id < 0) {
        printf("Colectivo no soportado por el sistema.\n");
        return ID_TOO_LARGE;
    }  
    
    if(!valid_pos(new_pos)){
        return NEW_POS_INVALID;
    }
    
    if(dist(new_pos, actual_pos) > 1){
        printf("No te podes mover mas de 1 lugar por turno.\n");
       return NEW_POS_FAR_AWAY;
    }
  
    pthread_mutex_lock(&semaphore_mutex);
    if(hasSemaphore(new_pos) && isVRedHGreen(semps[(semps_hash[new_pos.y][new_pos.x])]) 
	&& actual_pos.x == new_pos.x) {
	printf("No se puede avanzar, semaforo en rojo.\n");
        return RED_LIGHT_ON;
    }
    pthread_mutex_unlock(&semaphore_mutex);
    
    if(tiles[new_pos.y][new_pos.x] == TRUE){
        printf("No se puede avanzar, nueva posicion esta ocupada.\n");
        return NEW_POS_ALREADY_OCCUPIED;
    }

    if((aux=new_pos.x - actual_pos.x) != 0) {
        if(new_pos.y%6==0  && aux != 1) {
            printf("CONTRAMANO.\n");
            return WRONG_WAY;
        }
        if((new_pos.y%3==0 || new_pos.y==XDIM-1) /*&& aux != -1*/) {
            printf("CONTRAMANO.\n");
            return WRONG_WAY;
        }
    } else if((aux=new_pos.y - actual_pos.y) != 0) {
        if((new_pos.x%6==0 || new_pos.x==YDIM-1 ) /*&& aux != 1*/) {
            printf("CONTRAMANO.\n");
            return WRONG_WAY;
        }
        if(new_pos.x%3==0 && aux != -1) {
            printf("CONTRAMANO.\n");
            return WRONG_WAY;
        }
    }
	
    buses[fd][id] = new_pos;
    pthread_mutex_unlock(&map_mutex);
    tiles[actual_pos.y][actual_pos.x] = FALSE;
    tiles[new_pos.y][new_pos.x] = TRUE;
    pthread_mutex_unlock(&map_mutex);
    move_request_ack(session_line,fd,id);
    return id;
}

int
valid_pos(point_t pos){
    
    if(pos.x >= XDIM || pos.x < 0 
        || pos.y >= YDIM || pos.y < 0){
            printf("Posicion fuera las dimensiones de la ciudad.\n");
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
    sem->state = !sem->state;
}

            
         
