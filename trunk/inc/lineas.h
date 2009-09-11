#ifndef __LINEAS_H__
#define __LINEAS_H__

void * line_listener(); 

void * new_bus(int index);

void move_ack(int fd, int id); 

void insert_ack(int fd, int id);

#endif
