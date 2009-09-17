#ifndef __UTIL_H__
#define __UTIL_H__

/* Funcion que calcula la distancia entre 2 puntos */
float dist(point_t a,point_t b);

/* Cuenta la cantidad de digitos de un numero */
int digit_count(int n);

/* Funcion que pasa de integer a un vector de chars */
char * itoa( int n);
#endif
