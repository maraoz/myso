/**
 * protocol.c
 * author: maraoz
 * date: 2009-08-28
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include "../inc/typedef.h"
#include "../inc/protocol.h"
#include "../inc/util.h"


/**
 * http://www.ecst.csuchico.edu/~beej/guide/ipc/
 */

#define SESSION_MAX 100
int is_core;


#define USED 0
#define WRITE 1
#define READ 2


int sessions[SESSION_MAX][3]; // USED, WRITE, READ
int msqid_singleton;
package_t error_package = {-1, -1, -1, {-1, -1}};

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

#define SH_PACKAGE_MAX 10
typedef struct sh_pck {
    int used;
    package_t package;
} sh_package_t;

typedef struct shm_descr {
    int shmid;
    sh_package_t * data;
    int semaphore_id;
} shm_descriptor_t;

int s_w_init(void) {
    return 0;
}

session_t s_w_open(int other) {
    session_t new_session = get_session();
    char * name;
    key_t key = is_core?other:getpid();

    int size = SH_PACKAGE_MAX * sizeof(sh_package_t);
    shm_descriptor_t * desc_r = malloc(sizeof(shm_descriptor_t));
    shm_descriptor_t * desc_w = malloc(sizeof(shm_descriptor_t));

    int flag = 0666 | IPC_CREAT;
    desc_r->shmid = shmget(2*key, size, flag);
    desc_r->semaphore_id = semget(2*key, 1, flag);
    desc_w->shmid= shmget(2*key+1, size, flag);
    desc_w->semaphore_id = semget(2*key+1, 1, flag);

    if (desc_r->shmid == -1 | desc_w->shmid == -1) {
        printf("falla el shmget\n");
        perror("shmget");
        free(desc_r);free(desc_w);
        return -1;
    }

    desc_r->data = shmat(desc_r->shmid, (void *)0, 0);
    desc_w->data = shmat(desc_w->shmid, (void *)0, 0);

    if (desc_r->data == (sh_package_t *)(-1) ||
        desc_w->data == (sh_package_t *)(-1)) {
        printf("falla el shmat\n");
        free(desc_r);free(desc_w);
        return -1;
    }
    int i;
    for (i=0; i<SH_PACKAGE_MAX; i++){
        desc_w->data[i].used = FALSE;
    }
    sessions[new_session][READ] = (int) desc_r;
    sessions[new_session][WRITE] = (int) desc_w;
    if (commit_session(new_session) != -1)
        return new_session;
    else
        return -1;
}

int s_w_close(session_t session) {
    shm_descriptor_t * desc_r = (shm_descriptor_t *) sessions[session][READ];
    shm_descriptor_t * desc_w = (shm_descriptor_t *) sessions[session][WRITE];

    if(shmdt(desc_r->data) == -1 || shmdt(desc_w->data) == -1) {
        printf("fallo el shm_deattatch");
        return -1;
    }

    shmctl(desc_r->shmid, IPC_RMID, NULL);
    semctl(desc_r->semaphore_id, 0, IPC_RMID);
    shmctl(desc_w->shmid, IPC_RMID, NULL);
    semctl(desc_w->semaphore_id, 0, IPC_RMID);

    return 0;

};
int s_w_write(session_t session_id, package_t package) {
    struct sembuf sb = {0, 1, 0};

    shm_descriptor_t * shmdp = (shm_descriptor_t *) sessions[session_id][WRITE];
    sh_package_t * mem_zone = shmdp->data;

    // search for a free space in shared memory to place package
    int i;
    int found_free_zone = FALSE;
    for(i=0; i<SH_PACKAGE_MAX; i++) {
        if (mem_zone[i].used == FALSE) {
            found_free_zone = TRUE;
            break;
        }
    }
    // if no free space was found, return error
    if (! found_free_zone) {
        return -1;
    }

    // if a free memory zone was found put the package in there and return
    mem_zone[i].package = package;
    mem_zone[i].used = TRUE;
    // and set the resource as available
    if (semop(shmdp->semaphore_id, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }

    return 0;
};

package_t s_w_read(session_t session_id) {
    struct sembuf sb = {0, -1, 0};

    shm_descriptor_t * shmdp = (shm_descriptor_t *) sessions[session_id][READ];
    sh_package_t * mem_zone = shmdp->data;
    package_t ret;
    // search for a used space to get a package from it
    int i;
    int found_free_zone = FALSE;
    // only if there is one such resource available
    if (semop(shmdp->semaphore_id, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
    for(i=0; i<SH_PACKAGE_MAX; i++) {
        if (mem_zone[i].used == TRUE) {
            mem_zone[i].used = FALSE;
            ret = mem_zone[i].package;
            return ret;
        }
    }
    printf("error en s_w_read!!!\n");
    return error_package;
};

/**
 * FIFOs
 */
int f_w_init(void) {};

session_t f_w_open(int other) {
    char name1[31] = {'c'};
    char name2[31];
    char * other_name;
    int fd_read, fd_write;

    other_name = itoa(other);
    strcat(name1, other_name);
    strcat(other_name,"c");
    strncpy(name2, other_name, 30);

    printf("%s, %s\n", name1, name2);

    if (mknod(name1, S_IFIFO | 0666 , 0) == -1) {
        printf("error al crear un fifo\n");
    }
    if (mknod(name2, S_IFIFO | 0666 , 0) == -1) {
        printf("error al crear un fifo\n");
    }

    free(other_name);
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
    if (commit_session(new_session) != -1) {
        return new_session;
    }
    else
        return -1;
}

int m_w_close(session_t session) {
    sessions[session][USED] = FALSE;
    return 0;
}

int m_w_write(session_t session_id, package_t package) {

    int msqid = msqid_singleton;
    long msg_key = sessions[session_id][WRITE];

    q_msg_t queue_message;
    queue_message.mtype = msg_key;
    queue_message.content = package;
    int aux = msgsnd(msqid, &queue_message, sizeof(package_t), 0);
   
    if (aux == -1)
        perror("m_w_write");
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
        return error_package;
    }
}






/**
 * GENERAL
 */


int program_ipc_method;

int (*init_fns[4]) (void) = {s_w_init,f_w_init,k_w_init,m_w_init};
int w_init(int ipc_type, int mode) {
    program_ipc_method = ipc_type;

    is_core = (mode == CORE);

    init_sessions();
    return init_fns[program_ipc_method]();
}

session_t (*open_fns[4]) (int) = {s_w_open,f_w_open,k_w_open,m_w_open};
session_t w_open(int other) {
    return open_fns[program_ipc_method](other);
}


int (*close_fns[4]) (session_t) = {s_w_close,f_w_close,k_w_close,m_w_close};
int w_close(session_t session) {
    return close_fns[program_ipc_method](session);
}

int (*write_fns[4]) (session_t, package_t) =
    {s_w_write,f_w_write,k_w_write,m_w_write};
int w_write(session_t session_id, package_t package) {
    return write_fns[program_ipc_method](session_id, package);
}

package_t (*read_fns[4]) (session_t) = {s_w_read,f_w_read,k_w_read,m_w_read};
package_t w_read(session_t session_id) {
    return read_fns[program_ipc_method](session_id);
}



/**
 * TESTCASES
 */

int _main(void) {

    printf("inicializando transporte...");
    if (w_init(SHARED_MEMORY, LINE) == -1)
        printf("fallo el init\n");
    else
        printf("OK!\n");

    printf("abriendo puerta de entrada (open)...");
    session_t sid = w_open(getpid());
    if (sid == -1)
        printf("fallo el open...\n");
    else
        printf("OK!\n");

    package_t pck;

    printf("mandando 1...");
    pck.msg_id = 10;
    pck.id_line = 2;
    pck.id_bus = 3;
    pck.point.x = pck.point.y = 13;
    if (w_write(sid, pck) == -1)
        printf("fallo write 1...\n");
    else
        printf("OK!\n");

    printf("mandando 2...");
    pck.msg_id = 11;
    pck.id_line = 20;
    pck.id_bus = 33;
    pck.point.x = pck.point.y = 10;
    if (w_write(sid, pck) == -1)
        printf("fallo write 2...\n");
    else
        printf("OK!\n");

    package_t rcv;
    int i = 3;
    while (i--) {
        rcv = w_read(sid);
        printf("{msg_id: %d, id_line: %d, id_bus: %d, point: (%d, %d)}\n",
            rcv.msg_id, rcv.id_line, rcv.id_bus, rcv.point.x, rcv.point.y );
    }

    printf("closing connection, (close)...");
    if (w_close(sid) == -1)
        printf("Error al cerrar la conexion!\n");
    else
        printf("OK!\n");


    return 0;
}




