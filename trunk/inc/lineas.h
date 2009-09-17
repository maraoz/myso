#ifndef __LINEAS_H__
#define __LINEAS_H__

/* funcion que se encarga de escuchar del core */
void * line_listener(); 

/* funcion que crea un nuevo autobus con ID @index */
void * new_bus(int index);

/* funcion que actualiza la posicion del colectivo @idb de 
 * la linea @idl */
void move_ack(int idl, int idb); 

/* funcion que actualiza el estado inicial del colectivo @idb de 
 * la linea @idl */
void insert_ack(int idl, int idb);

/* funcion que inserta un nuevo pasajero en la linea @idl, con 
 * parada de subida @start y parada de bajada @stop */
void new_pax(int idl, point_t start, point_t stop);

/* funcion que termina la linea de ID @idl */
void end_line(int idl);

#endif
