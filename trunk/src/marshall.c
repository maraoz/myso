#include "../inc/typedef.h"
#include include <dirent.h>

#define CD_NO 0
#define CD_INSERT_BUS 1
#define CD_MOVE_BUS 2
#define CD_VALID_POS 3
#define CD_INIT 4

void
insert_request(int idl, int idb, point_t pos)
{
    package_t data;

    data.msg_id = CD_INSERT_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = pos;
    
    w_write(data);
}

insert_bus_ack
move_request_ack

void
move_request_ack(int idl, int idb, point_t new_pos)
{
    package_t data;

    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = new_pos;

    send(data);
}

void
receive()
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
        default: /* */
    }
}


/*** Datos que tiene que albergar un archivo de linea:
**	cantidad de colectivos. [ int ]
**	horarios de salida. [ int* ]
**	recorrido [ point_t* ]
**	cantidad de paradas. [ int ]
**	posiciones de las paradas. [ point_t* ]
*/

int
get_path(int fd_b, point_t *)
{
    





}



