#ifndef _CORE_H
#define _CORE_H

/*funcion que dice si una posicion esta dentro de los limites de la ciudad */
int valid_pos(point_t pos);

/* funcion que hace mover al autobus en caso de ser posible */
int move_bus(int id, point_t new_pos);

/* funcion que inserta un autobus en la ciudad */
int insert_bus(int * id, point_t pos); 

/* funcion que inicializa la ciudad */
int init(void);

#endif