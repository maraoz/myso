#include "../inc/typedef.h"

#define CD_NO 0
#define CD_INSERT_BUS 1
#define CD_MOVE_BUS

void
init()
{
    file = w_open();
}


void
insert_request(int * id, point_t pos)
{
    package_t data;

    data.msg_id = CD_INSERT_BUS;
    data.integer_p = id;
    data.point = pos;
    
    w_write(data);
}

void
move_request(int id, point_t new_pos)
{
    package_t data;

    data.msg_id = CD_MOVE_BUS;
    data.integer_p = id;
    data.point = new_pos;

    send(data);
}

void
receive(package_t data)
{
    switch(data.msg_id)
    {
	case CD_INSERT_BUS: insert_bus(data.integer_p, data.point);
	case CD_MOVE_BUS: move_bus*

}





