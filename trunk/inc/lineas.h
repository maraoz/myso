#ifndef __LINEAS_H__
#define __LINEAS_H__

void * line_listener(); 

void * new_bus(int index);

void move_ack(int fd, int id); 

void insert_ack(int fd, int id);

void new_pax(int fd, int id, point_t start, point_t stop);

void end_line(int line_id);

#endif
