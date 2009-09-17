
/* Funcion que inicializa el IPC del lado del core */
int m_init_core();


/* Funcion que inicializa el IPC del lado de cada linea */
int m_init_line();

/* Funcion que abre un canal de comunicacion por medio de IPC */
session_t openChannel(int channel);

/* Funcion que cierra el canal de comunicacion del IPC */
int closeChannel(int channel);

/* Funcion que envia un paquete al core solicitando insertar colectivo @idb de la linea @idl */
void insert_request(session_t session, int idl, int idb, point_t pos);

/* Funcion que envia un paquete a la linea @idl diciendole que se pudo insertar el colectivo @idb */
void insert_bus_ack(session_t session, int idl, int idb);

/* Funcion que envia un paquete a la linea @idl diciendole que el colectivo @idb se pudo mover */
void move_request_ack(session_t session, int idl, int idb);

/* Funcion que envia un paquete al core solicitando mover el colectivo @idb de la linea @idl */
void move_request(session_t session, int idl, int idb, point_t new_pos);

/* Funcion que envia un paquete a la linea @idl para insertar un pasajero */
void insert_pax_to_line(session_t session, int idl, point_t pos1, point_t pos2);

/* Funcion que envia un paquete para pedirle 2 paradas a la linea @idl */
void get_random_stops(session_t session, int idl);

/* Subfuncion de receive que segun el id del mensaje recibido llama a una
 * funcion del core */
void receive_core(package_t data);

/* Funcion que envia un paquete al core indicandole que un pasajero se bajo un colectivo */
void pax_downloaded(session_t session, int idl, int idb, point_t stop);

/* Funcion que envia un paquete al core indicandole que un pasajero abordo un colectivo */
void pax_uploaded(session_t session, int idl, int idb, point_t stop);

/* Subfuncion de receive que segun el id del mensaje recibido llama a una
 * funcion de las lineas */
void receive_lines(package_t data);

/* Funcion que deriva a la funcion correspondiente en base al id recibido 
 * al hacer read */
void receive();

/* Funcion para enviar el paquete de terminacion a una linea */
void delete_line(session_t sid, int line);
