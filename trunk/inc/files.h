#ifndef __FILES_H__
#define __FILES_H__

/* Funcion que abre el directorio con los archivos,
 * los abre uno a uno si corresponde y devuelve el FD. */
int openFiles(void);

/* Funcion que lee el recorrido de la linea y devuelve
 * la cantidad de puntos que lo componen y el recorrido mismo. */
int get_path(int fd, point_t ** path);

/* Funcion que lee del archivo y retorna la cantidad de 
 * colectivos que va a tener cada linea. */
int get_qty_buses(int fd);

/* Funcion que lee del archivo los tiempos en los que
 * deben salir los colectivos. */
int * get_times(int fd);

/* Funcion que lee la cantidad de paradas de la linea y devuelve
 * la cantidad de puntos que lo componen y los puntos. */
int get_stops(int fd, point_t ** stops);

/* Coloca el FD en el FD 3 cuplicandolo.
 * Deja el FD listo en la posicion 3 para saber mas tarde de donde leer. */
int preparefd(int fd);

/* Cierra el FD pasado como parametro. */
int closeFd(int fd);

/* Funcion que cierra el cirectorio. */
void closeDir(void);

#endif
