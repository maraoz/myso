#include "../inc/typedef.h"
#include "../inc/protocol.h"
#include "../inc/core.h"
#include "../inc/marshall.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <stdio.h>
#include <ncurses.h>


extern WINDOW *log_win;

#define CD_NO 0
#define CD_INSERT_BUS 1
#define CD_MOVE_BUS 2
#define CD_VALID_POS 3
#define CD_INIT 4
#define CD_PAX_DL 5
#define CD_DEL_STOPS 6
#define CD_MOVE_ACK 7
#define CD_INSERT_ACK 8
#define CD_NEW_PAX 9
#define CD_RND_STOPS 10

int ipc_selection = MESSAGE_QUEUE;
char * stringuis[] = {"SHARED_MEMORY","FIFO","SOCKET","MSG_QUEUE"};
int
m_init_core(){
    int result;
    result = w_init(ipc_selection, CORE);

    if(result == -1)
    wprintw(log_win, "+ERROR: ha fallado en init core\n");

    return result;
}

int
m_init_line(){
    int result;
    result = w_init(ipc_selection, LINE);
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado init line\n");

    return result;
}

session_t
openChannel(int channel){
    int result;
    result = w_open(channel);
    
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado en open, el channel es %d\n", channel);

    
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
	wprintw(log_win, "+ERROR: ha fallado en insert request\n");


}

void
insert_bus_ack(session_t session, int idl, int idb){

    package_t data;
    int result;
    
    data.msg_id = CD_INSERT_ACK;
    data.id_line = idl;
    data.id_bus = idb;

    if(DEBUG_MODE)
    wprintw(log_win, "TRACE: ESTOY POR ESCRIBIR EL APROBADO DEL INSERT\n");
    result = w_write(session, data);
    
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado en insert bus ack\n");

}

void
move_request_ack(session_t session, int idl, int idb){

    package_t data;
    int result;
    data.msg_id = CD_MOVE_ACK;
    data.id_line = idl;
    data.id_bus = idb;

    if(DEBUG_MODE)
	wprintw(log_win, "TRACE: ESTOY POR ESCRIBIR EL APROBADO DEL MOVE\n");
    result = w_write(session, data);
    
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado en move request ack\n");

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
	wprintw(log_win, "+ERROR: ha fallado en move request\n");


}

void
pax_downloaded(session_t session, int idl, int idb, point_t stop){

    package_t data;
    int result;
    
    data.msg_id = CD_PAX_DL;
    data.id_line = idl;
    data.id_bus = idb;
    data.point = stop;

    result = w_write(session, data);
    
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado en download pax\n");

}

void 
insert_pax_to_line(session_t session, int idl, point_t pos1, point_t pos2){
    
    package_t data;
    int result;
    
    data.msg_id = CD_NEW_PAX;
    data.id_line = idl;
    data.point = pos1;
    data.point2 = pos2;

    result = w_write(session, data);
    
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado en insert pax\n");
}


void
get_random_stops(session_t session, int idl){
    
    package_t data;
    int result;
    
    data.msg_id = CD_RND_STOPS;
    data.id_line = idl;

    result = w_write(session, data);
    
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado en get random stops\n");
    
}

void
deliver_stops(session_t session, int idl, point_t stop1, point_t stop2){
 
    package_t data;
    int result;
    
    data.msg_id = CD_DEL_STOPS;
    data.id_line = idl;
    data.point = stop1;
    data.point2 = stop2;

    result = w_write(session, data);
    
    if(result == -1)
	wprintw(log_win, "+ERROR: ha fallado en deliver stops\n");   
}


void
receive_core(package_t data)
{
    int code;

    switch(data.msg_id)
    {
        case CD_INSERT_BUS: code = insert_bus(data.id_line, data.id_bus, data.point);break;
        case CD_MOVE_BUS: code = move_bus(data.id_line, data.id_bus, data.point);break;
        case CD_VALID_POS: code = valid_pos(data.point);break;
        case CD_INIT: code = init(); break;
        case CD_PAX_DL: code = pax_get_of_bus(data.id_line,data.point); break;
        case CD_DEL_STOPS: code = set_new_pax(data.id_line, data.point, data.point2);break;
        default: /* */;
    }
}

void
receive_lines(package_t data)
{
    int code;
    
    switch(data.msg_id)
    {
        case CD_INSERT_ACK: code = insert_ack(data.id_line, data.id_bus);break;
        case CD_MOVE_ACK: code = move_ack(data.id_line, data.id_bus);break;
        case CD_NEW_PAX: code = new_pax(data.id_line, data.id_bus, data.point, data.point2); break;
        case CD_RND_STOPS: code = calculate_stops(data.id_line); break;
        default: /* */;
    }
}

void
receive(session_t session)
{
    package_t data;
                
    data = w_read(session);

    if(DEBUG_MODE)
	wprintw(log_win,"TRACE: LLEGUE HASTA ANTES DEL SWITCH\n");
    if(DEBUG_MODE)
	wprintw(log_win,"TRACE: MSG_ID = %d\n",data.msg_id);
    if(data.msg_id == -1){
        if(DEBUG_MODE){
	    wprintw(log_win,"TRACE: EL MENSAJE ES EL ESPERADO\n");
	}
    }
    switch(data.msg_id)
    {
        case CD_INSERT_BUS:
        case CD_MOVE_BUS:
        case CD_VALID_POS:
        case CD_INIT:
        case CD_PAX_DL:
        case CD_DEL_STOPS: if(DEBUG_MODE) wprintw(log_win,"TRACE: LLEGUE HASTA RECEIVE CORE\n"); receive_core(data); break;
        case CD_INSERT_ACK:
        case CD_MOVE_ACK:
        case CD_NEW_PAX:
        case CD_RND_STOPS: if(DEBUG_MODE) wprintw(log_win,"TRACE: LLEGUE HASTA RECEIVE LINES\n");receive_lines(data); break;
        default: /* */;
    }
}
    
