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

int
m_init_core(){
    int result;
    
    result = w_init(MESSAGE_QUEUE, CORE);

    if(result == -1)
	printf("ha fallado en init core\n");

    
    return result;
}

int
m_init_line(){
    int result;
    
    result = w_init(MESSAGE_QUEUE, LINE);

    if(result == -1)
	printf("ha fallado init line\n");

    
    return result;
}

session_t
openChannel(int channel){
    int result;
    
    
    result = w_open(channel);
    
    if(result == -1)
	printf("ha fallado en open, el channel es %d\n", channel);

    
    return result;
}

int
closeChanel(int channel){
    return w_close(channel);
}

void
insert_request(session_t session, int idl, int idb, point_t pos)
{
    package_t data;
    int result;

    data.msg_id = CD_INSERT_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = pos;
    
    
    result = w_write(session, data);

    if(result == -1)
	printf("ha fallado en insert request\n");


}

void
insert_bus_ack(session_t session, int idl, int idb){

    package_t data;
    int result;
    
    data.msg_id = CD_INSERT_ACK;
    data.id_line = idl;
    data.id_bus = idb;

    if(DEBUG_MODE)
    printf("TRACE: ESTOY POR ESCRIBIR EL APROBADO DEL INSERT\n");
    result = w_write(session, data);
    
    if(result == -1)
	printf("ha fallado en insert bus ack\n");

}

void
move_request_ack(session_t session, int idl, int idb){

    package_t data;
    int result;
    data.msg_id = CD_MOVE_ACK;
    data.id_line = idl;
    data.id_bus = idb;

    if(DEBUG_MODE)
    printf("TRACE: ESTOY POR ESCRIBIR EL APROBADO DEL MOVE\n");
    result = w_write(session, data);
    
    if(result == -1)
	printf("ha fallado en move request ack\n");

}

void
move_request(session_t session, int idl, int idb, point_t new_pos)
{
    package_t data;
    int result;
    data.msg_id = CD_MOVE_BUS;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = new_pos;

    result = w_write(session, data);
    
    if(result == -1)
	printf("ha fallado en move request\n");


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
        case CD_INSERT_ACK: code = insert_ack(data.id_line, data.id_bus);break;
        case CD_MOVE_ACK: code = move_ack(data.id_line, data.id_bus);break;
        default: /* */;
    }
}

void
receive(session_t session)
{
    package_t data;
                
    data = w_read(session);

    if(DEBUG_MODE)
    printf("TRACE: LLEGUE HASTA ANTES DEL SWITCH\n");
    if(DEBUG_MODE)
    printf("TRACE: MSG_ID = %d\n",data.msg_id);
    if(data.msg_id == CD_MOVE_ACK){
        if(DEBUG_MODE)
        printf("TRACE: EL MENSAJE ES EL ESPERADO\n");
    }
    switch(data.msg_id)
    {
        case CD_INSERT_BUS:
        case CD_MOVE_BUS:
        case CD_VALID_POS:
        case CD_INIT: receive_core(data); break;
        case CD_INSERT_ACK:
        case CD_MOVE_ACK: if(DEBUG_MODE) printf("TRACE: LLEGUE HASTA EL SWITCH\n");receive_lines(data); break;
        default: /* */;
    }
}
    
