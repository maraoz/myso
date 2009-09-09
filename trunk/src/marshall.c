#include "../inc/typedef.h"
#include "../inc/protocol.h"
#include "../inc/core.h"
#include <dirent.h>

#define CD_NO 0
#define CD_INSERT_BUS 1
#define CD_MOVE_BUS 2
#define CD_VALID_POS 3
#define CD_INIT 4

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
    w_open(channel);
}

void
insert_request(int idl, int idb, point_t pos)
{
    package_t data;

    data.msg_id = CD_INSERT_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = pos;
    
    w_write(idl, data);
}

void
insert_bus_ack(int idl, int idb){

    package_t data;

    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;

    w_write(idl, data);
}

void
move_request_ack(int idl, int idb){

    package_t data;

    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;

    w_write(idl, data);
}

void
move_request(int idl, int idb, point_t new_pos)
{
    package_t data;

    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = new_pos;

    w_write(idl, data);
}

void
receive_core()
{
    int code;
    package_t data;
    
    data = w_read();
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
receive_lines()
{
    int code;
    package_t data;
    
    data = w_read();
    switch(data.msg_id)
    {
        case CD_INSERT_BUS: code = insert_ack(data.id_line);break;
        case CD_MOVE_BUS: code = move_ack(data.id_line, data.id_bus);break;
        default: /* */;
    }
}


