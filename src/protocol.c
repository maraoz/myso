
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
        char * content;
    } msg_t;

typedef int session_t;

session_t w_open(int who) {
    key_t key; /* key to be passed to msgget() */ 
    int msgflg; /* msgflg to be passed to msgget() */ 
    int msqid; /* return value from msgget() */ 

    key = 1928;
    msgflg = IPC_CREAT | 0666;
    if ((msqid = msgget(key, msgflg)) == -1)
        return -1;
    return msqid;
}

int w_close(session_t session) {
    return msgctl(session, IPC_RMID, NULL);
}

int w_write(session_t session, msg_t msg) {
    return msgsnd(session, &msg, sizeof(msg), 0);
}

msg_t w_read(session_t session) {
    msg_t ret;
    msgrcv(session, &ret, sizeof(ret), 2, 0);
    return ret;
}


int main(void) {
    session_t sid = w_open(1);
    if (sid == -1){
        printf("error al abrir la IPC\n");
    }
    msg_t msg = {2, "hola como te va?!\n"};
    if (w_write(sid, msg) != 0) {
        printf("error al escribir en el IPC\n");
    }
    msg_t rcvd_msg;
    rcvd_msg = w_read(sid);
    printf("%s", rcvd_msg.content);

    w_close(sid);
    return 0;
}