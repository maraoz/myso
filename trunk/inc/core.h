#ifndef __CORE_H__
#define __CORE_H__

#define CUADRAS 6
#define TILES_CUADRAS 2
#define XDIM (TILES_CUADRAS+1)*CUADRAS+1
#define YDIM (TILES_CUADRAS+1)*CUADRAS+1
#define TRUE 1
#define FALSE 0
#define V_GREEN 1
#define V_RED   0
#define H_GREEN V_RED
#define H_REG V_GREEN


#define FD_TOO_LARGE 1
#define ID_TOO_LARGE 1
#define NEW_POS_INVALID 1
#define BLOCKED_SLOT 1
#define BUS_ALREADY_IN_SLOT 1
#define NEW_POS_FAR_AWAY 1
#define RED_LIGHT_ON 1
#define NEW_POS_ALREADY_OCCUPIED 1
#define WRONG_WAY 1


/*funcion que dice si una posicion esta dentro de los limites de la ciudad */
int valid_pos(point_t pos);

/* funcion que hace mover al autobus en caso de ser posible */
int move_bus(session_t session, int id, point_t new_pos);

/* funcion que inserta un autobus en la ciudad */
int insert_bus(session_t session, int id, point_t pos); 

int set_new_pax(int idl, point_t stop_up, point_t stop_down);

int pax_get_of_bus(int idl, point_t stop);

/* funcion que inicializa la ciudad */
int init(void);

/* funcion que devuelve TRUE o FALSE dependiendo del estado del semaforo */
int isVRedHGreen(semaphore s);

/* funcion que retorna TRUE si en pos hay un semaforo y FALSE caso contrario */
int hasSemaphore(point_t pos);

void * listen(int index);

void * pax_creation();

void switch_semaphore(semaphore *s);


#endif
