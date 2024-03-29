/*
 * Header containing type definitions for project.
 */ 

#ifndef _TYPEDEF_H
#define _TYPEDEF_H

typedef unsigned char boolean;


// Core
typedef struct point_t {

    int x;
    int y;
} point_t;

typedef struct {
    point_t * path;
    point_t * stops;
    int path_length;
    int stops_length;
} buses_line;

typedef struct {
    point_t up;
    point_t down;
    int line;
    int used;
} person_t;

// protocol
typedef int session_t;


// marshall
typedef struct
{
    int msg_id;			/* id del mensaje que se envia */
    int id_line;		/*  */ 
    int id_bus;			/*  */
    point_t point;		/* posible posicion del colectivo */
    point_t point2;
} package_t;



typedef struct
{
    int cantFiles;
    session_t * filesId;
} files_t;



typedef struct {
    point_t pos;
    int state;
}semaphore;

typedef struct {
    int * buffer;
    int qty;
} Tfiles;


#define DEBUG_MODE 0

#endif
