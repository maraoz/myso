#ifndef __CORE_H__
#define __CORE_H__


/* Valores generales predefinidos usados dentro de core.c y maincore.c */
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


/* Valores de retorno de las funciones cuando no se cumplen 
 * todas las validaciones */
#define FD_TOO_LARGE 1
#define ID_TOO_LARGE 1
#define NEW_POS_INVALID 1
#define BLOCKED_SLOT 1
#define BUS_ALREADY_IN_SLOT 1
#define NEW_POS_FAR_AWAY 1
#define RED_LIGHT_ON 1
#define NEW_POS_ALREADY_OCCUPIED 1
#define WRONG_WAY 1


/* funcion que dice si la posicion @pos se encuentra dentro de los 
 * limites de la ciudad */
int valid_pos(point_t pos);

/* funcion que mueve el autobus @idb de la linea @idl a la posicion
 * @new_pos, en caso de ser posible */
int move_bus(int idl, int idb, point_t new_pos);

/* funcion que inserta el autobus @idb de la linea @idl en la posicion
 * @pos, en caso de ser posible */
int insert_bus(int idl, int idb, point_t pos); 

/* funcion que crea un pasajero nuevo en la linea @idl, con
 * parada de subida @stop_up y parada de bajada @stop_down */
int set_new_pax(int idl, point_t stop_up, point_t stop_down);

/* funcion que indica que un pasajero se bajo del autobus 
 * @idb de la linea @idl en la parada @stop */
int pax_get_of_bus(int idl, int idb, point_t stop);

/* funcion que indica que un pasajero se subio al autobus 
 * @idb de la linea @idl en la parada @stop */
int pax_get_on_bus(int idl, int idb, point_t stop);

/* funcion que inicializa el core */
int init(void);

/* funcion que devuelve TRUE o FALSE dependiendo del estado del semáforo @s*/
int isVRedHGreen(semaphore s);

/* funcion que retorna TRUE si en @pos hay un semaforo y FALSE caso contrario */
int hasSemaphore(point_t pos);

/* funcion que se encarga de escuchar la session de la linea que tiene 
 * como id @index*/
void * core_listen(int index);

/* funcion que se encarga de crear pasajeros cada cierto intervalo de tiempo */
void * pax_creation();

/* funcion que se encarga de escuchar el teclado */
void * keyboard_listen();

/* funcion que invierte el estado del semaforo apuntado por @s */
void switch_semaphore(semaphore *s);


#endif
