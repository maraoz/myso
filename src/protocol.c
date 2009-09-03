/**
 * protocol.c
 * author: maraoz
 * date: 2009-08-28
 */



#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../inc/typedef.h"

/**
 * http://www.ecst.csuchico.edu/~beej/guide/ipc/
 */

#define SHARED_MEMORY 0
#define FIFO 1
#define SOCKET 2
#define MESSAGE_QUEUE 3
int program_ipc_method;

int w_init(int ipc_type) {
    program_ipc_methods = ipc_type;
    switch(program_ipc_methods) {
        case SHARED_MEMORY:
            s_w_init(); break;
        case FIFO:
            f_w_init(); break;
        case SOCKET:
            s_w_init(); break;
        case MESSAGE_QUEUE:
            m_w_init(); break;
        default:
            printf("Método IPC inválido\n");
    }
}

session_t w_open(int other) {
}

int w_close(session_t session) {
}

int w_write(session_t session_id, package_t package) {
}

package_t w_read(void) {
}


/**
 * Shared Memory
 */

/**
 * FIFOs
 */

/**
 * Sockets
 */

/**
 * Message Queue
 */

typedef struct msg {
        long mtype;
        package_t content;
    } q_msg_t;


#define TRUE 1
#define FALSE 0

#define USED 0
#define CONTENT 1
#define SESSION_MAX 100

session_t sessions[SESSION_MAX][2]; // USED, CONTENT
int msqid_singleton;
session_t get_session(void) {
    int i;
    for (i=0; i< SESSION_MAX; i++) {
        if (sessions[i][USED] == FALSE) {
            return i;
        }
    }
    return -1;
}

int commit_session(session_t session) {
    if (sessions[session][USED] == FALSE) {
        sessions[session][USED] = TRUE;
    } else {
        return -1;
    }
}


int w_init(void) {
    key_t key;
    int msgflg, msqid;
    key = 1928;

    msgflg = IPC_CREAT | 0666;
    msqid = msgget(key, msgflg);
    if ( msqid == -1)
        return -1;
    msqid_singleton = msqid;
}

session_t w_open(int other) {

    session_t new_session = get_session();
    sessions[new_session][CONTENT] = other;
    if (commit_session(new_session) != -1)
        return new_session;
    else
        return -1;
}

int w_close(session_t session) {
    sessions[session][USED] = FALSE;
}

int w_write(session_t session_id, package_t package) {
    int msqid = msqid_singleton;
    long msg_key = sessions[session_id][CONTENT];
    q_msg_t queue_message;
    queue_message.mtype = msg_key;
    queue_message.content = package;
    return msgsnd(msqid, &queue_message, sizeof(package_t), 0);
}

package_t w_read(void) {
    package_t ret;
    int msqid = msqid_singleton;
    q_msg_t q_message;
    long id_to_filter = getpid();
    if (msgrcv(msqid, &q_message, sizeof(package_t), id_to_filter, 0) != -1)
    {
        ret = q_message.content;
        return ret;
    } else {
        printf("Error al leer\n");
        exit(0);
    }
}


int main(void) {

    w_init();
    session_t sid = w_open(getpid());
    package_t pck;

    pck.msg_id = 10;
    pck.id_line = 2;
    pck.id_bus = 3;
    pck.point.x = pck.point.y = 13;
    w_write(sid, pck);
    printf("mandando...\n");
    package_t rcv = w_read();
    printf("recibido msg_id = %d,\nid_line %d,\nid_bus %d,\n(%d, %d)\n",
        rcv.msg_id, rcv.id_line, rcv.id_bus, rcv.point.x, rcv.point.y );


    return 0;
}

