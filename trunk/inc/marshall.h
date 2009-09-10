

int m_init_core();

int m_init_line();

int openChannel(int channel);

int closeChanel(int channel);

void insert_request(session_t session, int idl, int idb, point_t pos);

void insert_bus_ack(session_t session, int idl, int idb);

void move_request_ack(session_t session, int idl, int idb);

void move_request(session_t session, int idl, int idb, point_t new_pos);

void receive_core(package_t data);

void receive_lines(package_t data);

void receive();
