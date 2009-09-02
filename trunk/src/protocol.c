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


#define TRUE 1
#define FALSE 0

#define USED 0
#define READ 1
#define WRITE 2
#define SESSION_MAX 100

session_t sessions[SESSION_MAX][3]; // USED, WRITE, READ

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


session_t w_open(int other) {

    session_t new_session = get_session();

    /* For writing */
    key_t key;
    int msgflg, msqid;
    key = 1928+other*2;

    msgflg = IPC_CREAT | 0666;
    msqid = msgget(key, msgflg);
    if ( msqid == -1)
        return -1;
    sessions[new_session][WRITE] = msqid;

    /* For reading */
    key = other*2+1927;
    msqid = msgget(key, msgflg);
    if ( msqid == -1)
        return -1;
    sessions[new_session][READ] = msqid;
    if (commit_session(new_session) != -1)
        return new_session;
    else
        return -1;
}

int w_close(session_t session) {
    return msgctl(session, IPC_RMID, NULL);
}

int w_write(session_t session_id, msg_t msg) {
    int msqid = sessions[session_id][WRITE];
    return msgsnd(msqid, &msg, sizeof(msg), 0);
}

msg_t w_read(session_t session_id) {
    msg_t ret;
    int msqid = sessions[session_id][READ];
    msgrcv(session_id, &ret, sizeof(ret), 2, 0);
    return ret;
}


int main(void) {
    session_t sid = w_open(12);
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

