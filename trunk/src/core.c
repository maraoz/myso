#include "../inc/typedef.h"
#include "../inc/core.h"

#define CUADRAS 6
#define XDIM 3*CUADRAS
#define YDIM 3*CUADRAS
#define TRUE 1
#define FALSE !TRUE

boolean tiles[YDIM][XDIM];
point_t buses[XDIM*YDIM];
int next_id=0;

int
init(void) {
    
    int i,j;
    
    /* LLeno la matriz de la ciudad, donde hay una manzana pongo TRUE, asi siempre
    está ocupada y no se puede pasar por encima de una manzana. */
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

int
insert_bus(int * id, point_t pos){
    
    if(!valid_pos(pos)){
        return NEW_POS_INVALID;
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
    buses[next_id] = pos;
    (*id) = next_id;
    next_id++;
    return next_id-1;
        
}


int
move_bus(int id, point_t new_pos){
    point_t actual_pos = buses[id];
    if(!valid_pos(new_pos)){
        return NEW_POS_INVALID;
    if(dist(new_pos, actual_pos) > 1){
        printf("No te podes mover mas de 1 lugar por turno.\n");
        return NEW_POS_FAR_AWAY;
    }
    if(tile[new_pos.y][new_pos.x]){
        printf("No se puede avanzar, nueva posición está ocupada.\n");
        return NEW_POS_ALREADY_OCCUPIED;
    }
    /* aca habria que chequear que esto sea atomico */
    bus[id] = new_pos;
    tile[actual_pos.y][actual_pos.x] = FALSE;
    tile[new_pos.y][new_pos.x] = TRUE;
    return id;
}

int
valid_pos(point_t pos){
    
    if(pos.x >= XIMD || pos.x < 0 
        || pos.y >= YDIM || pos.y < 0){
            printf("Posición fuera las dimensiones de la ciudad.\n");
            return FALSE;
    }
    return TRUE;
}
            
            
         