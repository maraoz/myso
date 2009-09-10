#include "../inc/typedef.h"
#include "../inc/protocol.h"
#include "../inc/core.h"
#include "../inc/marshall.h"
#include <stdio.h>
#include <dirent.h>
#include <stdio.h>

#define CD_NO 0
#define CD_INSERT_BUS 1
#define CD_MOVE_BUS 2
#define CD_VALID_POS 3
#define CD_INIT 4
#define CD_MOVE_ACK 5
#define CD_INSERT_ACK 6

void
m_init_core(){
    w_init(MESSAGE_QUEUE, CORE);
}

void
m_init_line(){
    w_init(MESSAGE_QUEUE, LINE);
}

int
openChannel(int channel){
    return w_open(channel);
}

int
closeChanel(int channel){
    return w_close(channel);
}

void
insert_request(int idl, int idb, point_t pos)
{
    package_t data;
    int result;

    data.msg_id = CD_INSERT_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = pos;
   
    result = w_write(idl, data);
    
    if(result == -1)
	printf("ha fallado");
    else
	printf("no ha fallado");

}

void
insert_bus_ack(int idl, int idb){

    package_t data;
    int result;
    
    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;

    result = w_write(idl, data);
    
    if(result == -1)
	printf("ha fallado");
    else
	printf("no ha fallado");
}

void
move_request_ack(int idl, int idb){

    package_t data;
    int result;
    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;

    result = w_write(idl, data);
    
    if(result == -1)
	printf("ha fallado");
    else
	printf("no ha fallado");
}

void
move_request(int idl, int idb, point_t new_pos)
{
    package_t data;
    int result;
    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = new_pos;

    result = w_write(idl, data);
    
    if(result == -1)
	printf("ha fallado");
    else
	printf("no ha fallado");

}

void
receive_core(package_t data)
{
    int code;
//     package_t data;

//     data = w_read();
    switch(data.msg_id)
    {
        case CD_INSERT_BUS: code = insert_bus(data.id_line, data.id_bus, data.point);break;
        case CD_MOVE_BUS: code = move_bus(data.id_line, data.id_bus, data.point);break;
        case CD_VALID_POS: code = valid_pos(data.point);break;
        case CD_INIT: code = init(); break;
        default: /* */;
    }
}

void
receive_lines(package_t data)
{
    int code;
//     package_t data;

//     data = w_read();
    switch(data.msg_id)
    {
        case CD_INSERT_ACK: code = insert_ack(data.id_line);break;
        case CD_MOVE_ACK: code = move_ack(data.id_line, data.id_bus);break;
        default: /* */;
    }
}

void
receive()
{
    package_t data;

    data = w_read();
    printf("%d\n",data.msg_id);
    switch(data.msg_id)
    {
        case CD_INSERT_BUS:
        case CD_MOVE_BUS:
        case CD_VALID_POS:
        case CD_INIT: receive_core(data); break;
        case CD_INSERT_ACK:
        case CD_MOVE_ACK: receive_lines(data); break;
        default: /* */;
    }
}
    
