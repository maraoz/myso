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
    int msg_id;
    int * integer_p;
    char * character_p;
    int integer;
    point_t point;
} package_t;

// protocol
typedef int session_t;

#endif
