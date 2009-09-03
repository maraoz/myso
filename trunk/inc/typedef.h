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


// marshall
typedef struct
{
    int msg_id;			/* id del mensaje que se envia */
    int id_line;		/*  */ 
    int id_bus			/*  */
    point_t point;		/* posible posicion del colectivo */
} package_t;

typedef struct
{
    int cantFiles;
    session_t * filesId;
} files_t;


// protocol
typedef int session_t;

#endif
