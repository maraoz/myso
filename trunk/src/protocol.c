/**
 * protocol.c
 * author: maraoz
 * date: 2009-08-28
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../inc/typedef.h"
#include "../inc/protocol.h"


/**
 * http://www.ecst.csuchico.edu/~beej/guide/ipc/
 */

#define SESSION_MAX 100
int is_core;


#define USED 0
#define WRITE 1
#define READ 2

int digit_count(int n) {
    int i;
    if (n == 0)
        return 1;
    for (i=0; n != 0; i++) {
        n /= 10;
    }
    return i;
}

char * itoa( int n) {
    char * ret = calloc(11,sizeof(char));
    if (n == 0) {
        ret[0] = '0';
        return ret;
    }
    int i;
    int dc = digit_count(n);
    for (i=0; n != 0; i++) {
        ret[dc-i-1] = n % 10 + '0';
        n /= 10;
    }
    return ret;
}


int sessions[SESSION_MAX][3]; // USED, WRITE, READ
int msqid_singleton;

void init_sessions(void) {
    int i;
    for (i=0; i<SESSION_MAX; i++) {
        sessions[i][USED] = FALSE;
    }
}

session_t get_session(void) {
    session_t i;
    for (i=0; i< SESSION_MAX; i++) {
        if (sessions[i][USED] == FALSE) {
            return i;
        }
    }
    return (session_t)-1;
}

int commit_session(session_t session) {
    if (sessions[session][USED] == FALSE) {
        sessions[session][USED] = TRUE;
        return 0;
    } else {
        return -1;
    }
}

/**
 * Shared Memory
 */

int s_w_init(void) {}

session_t s_w_open(int other) {
//     session_t new_session = get_session();
// 
//     
// 
//     sessions[new_session][READ] = other;
//     sessions[new_session][WRITE] = other;
//     if (commit_session(new_session) != -1)
//         return new_session;
//     else
//         return -1;
//     shmget(key_t key, size_t size, int shmflg);
}

int s_w_close(session_t session) {};
int s_w_write(session_t session_id, package_t package) {};
package_t s_w_read(session_t session_id) {};

/**
 * FIFOs
 */
int f_w_init(void) {};

session_t f_w_open(int other) {
    char name1[31];
    char name2[31];
    char * other_name;
    int fd_read, fd_write;

    other_name = itoa(other);
    strncpy(name1, strcat("c-", other_name), 30);
    strncpy(name2, strcat(other_name, "-c"), 30);
    printf("%s, %s", name1, name2);

    if (mknod(name1, S_IFIFO | 0666 , 0) == -1) {
        printf("error al crear un fifo\n");
    }
    if (mknod(name2, S_IFIFO | 0666 , 0) == -1) {
        printf("error al crear un fifo\n");
    }

    fd_write = open(is_core?name1:name2, O_WRONLY);
    fd_read = open(is_core?name2:name1, O_RDONLY);
    session_t new_session = get_session();
    sessions[new_session][WRITE] = fd_write;
    sessions[new_session][READ] = fd_read;
    if (commit_session(new_session) != -1)
        return new_session;
    else
        return -1;
}

int f_w_close(session_t session) {
    int fd_read, fd_write;
    fd_read = sessions[session][READ];
    fd_write = sessions[session][WRITE];

    close(fd_read);
    close(fd_write);
    sessions[session][USED] = FALSE;
    return 0;
};
int f_w_write(session_t session_id, package_t package) {
    int fd_write = sessions[session_id][WRITE];
    return write(fd_write, package, sizeof(package));
};

package_t f_w_read(session_t session_id) {
    int fd_read = sessions[session_id][READ];
    int size = sizeof(package_t);

    package_t buff;
    package_t * buffp = &buff;
    package_t * ret = buffp;
    int count;
    while(size){
        count = read( fd_read, buffp, size );
        size -= count;
        buffp += count;
    }
    return *ret;
}

/**
 * Sockets
 */
int k_w_init(void) {};
session_t k_w_open(int other) {};
int k_w_close(session_t session) {};
int k_w_write(session_t session_id, package_t package) {};
package_t k_w_read(session_t session_id) {};

/**
 * Message Queue
 */

typedef struct msg {
        long mtype;
        package_t content;
    } q_msg_t;


int m_w_init(void) {
    key_t key;
    int msgflg, msqid;
    key = 1928;

    msgflg = IPC_CREAT | 0666;
    msqid = msgget(key, msgflg);
    if ( msqid == -1)
        return -1;
    msqid_singleton = msqid;
}

session_t m_w_open(int other) {

    session_t new_session = get_session();
    sessions[new_session][READ] = other;
    sessions[new_session][WRITE] = other;
    if (commit_session(new_session) != -1)
        return new_session;
    else
        return -1;
}

int m_w_close(session_t session) {
    sessions[session][USED] = FALSE;
    return 0;
}

int m_w_write(session_t session_id, package_t package) {
    printf("%d", session_id);
    int msqid = msqid_singleton;
    long msg_key = sessions[session_id][WRITE];
    q_msg_t queue_message;
    queue_message.mtype = msg_key;
    queue_message.content = package;
    int aux = msgsnd(msqid, &queue_message, sizeof(package_t), 0);
//     printf("%d", errno);
    return aux;
}

package_t m_w_read(session_t session_id) {
    package_t ret;
    int msqid = msqid_singleton;
    q_msg_t q_message;
    long id_to_filter;

    id_to_filter = is_core?1:getpid();

    if (msgrcv(msqid, &q_message, sizeof(package_t), id_to_filter, 0) != -1)
    {
        ret = q_message.content;
        return ret;
    } else {
        printf("Error al leer\n");
        exit(0);
    }
}






/**
 * GENERAL
 */


int program_ipc_method;

int w_init(int ipc_type, int mode) {
    program_ipc_method = ipc_type;

    is_core = (mode == CORE);

    init_sessions();

    switch(program_ipc_method) {
        case SHARED_MEMORY:
            s_w_init(); break;
        case FIFO:
            f_w_init(); break;
        case SOCKET:
            k_w_init(); break;
        case MESSAGE_QUEUE:
            m_w_init(); break;
        default:
            printf("Método IPC inválido\n");
    }
}

session_t w_open(int other) {
    // TODO: contemplate other IPCS
    return m_w_open(other);
}

int w_close(session_t session) {
    // TODO: contemplate other IPCS
    return m_w_close(session);
}

int w_write(session_t session_id, package_t package) {
    // TODO: contemplate other IPCS
    return m_w_write(session_id, package);
}

package_t w_read(session_t session_id) {
    // TODO: contemplate other IPCS
    return m_w_read(session_id);
}



/**
 * TESTCASES
 */
/*
int _main(void) {

    w_init(MESSAGE_QUEUE, LINE);
    session_t sid = w_open(getpid());
    package_t pck;

    printf("mandando 1...\n");
    pck.msg_id = 10;
    pck.id_line = 2;
    pck.id_bus = 3;
    pck.point.x = pck.point.y = 13;
    w_write(sid, pck);

    printf("mandando 2...\n");
    pck.msg_id = 11;
    pck.id_line = 20;
    pck.id_bus = 33;
    pck.point.x = pck.point.y = 10;
    w_write(sid, pck);

    package_t rcv = w_read(sid);
    printf("recibido msg_id = %d,\nid_line %d,\nid_bus %d,\n(%d, %d)\n",
        rcv.msg_id, rcv.id_line, rcv.id_bus, rcv.point.x, rcv.point.y );
    rcv = w_read(sid);
    printf("recibido msg_id = %d,\nid_line %d,\nid_bus %d,\n(%d, %d)\n",
        rcv.msg_id, rcv.id_line, rcv.id_bus, rcv.point.x, rcv.point.y );
    rcv = w_read(sid);
    printf("recibido msg_id = %d,\nid_line %d,\nid_bus %d,\n(%d, %d)\n",
        rcv.msg_id, rcv.id_line, rcv.id_bus, rcv.point.x, rcv.point.y );


    return 0;
}



*/
