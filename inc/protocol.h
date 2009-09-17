#ifndef _PROTOCOL_H
#define _PROTOCOL_H


#define TRUE 1
#define FALSE 0

#define SHARED_MEMORY 0
#define FIFO 1
#define SOCKET 2
#define MESSAGE_QUEUE 3

#define CORE 0
#define LINE 1

/* Funcion para inicializar el IPC que se elija y su modo*/
int w_init(int ipc_type, int mode);

/* Funcion para dejar lista la comunicacion entre los procesos */
session_t w_open(int other);

/* Funcion para cerrar la comunicacion por medio de IPC */
int w_close(session_t session);

/* Funcion para escribir en la zona que corresponda, segun el IPC que se use */
int w_write(session_t session_id, package_t package);

/* Funcion para leer de la zona que corresponda segun el IPC que se use */
package_t w_read(session_t session_id);


#endif
