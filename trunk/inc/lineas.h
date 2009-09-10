#ifndef __LINEAS_H__
#define __LINEAS_H__

void * new_bus(int index);

void move_ack(session_t session, int id); 

void insert_ack(int id);

#endif
