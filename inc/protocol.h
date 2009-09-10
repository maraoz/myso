
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








int w_init(int ipc_type, int mode);
session_t w_open(int other);
int w_close(session_t session);
int w_write(session_t session_id, package_t package);
package_t w_read();


#endif