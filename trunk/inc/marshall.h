

int m_init_core();

int m_init_line();

session_t openChannel(int channel);

int closeChannel(int channel);

void insert_request(session_t session, int idl, int idb, point_t pos);

void insert_bus_ack(session_t session, int idl, int idb);

void move_request_ack(session_t session, int idl, int idb);

void move_request(session_t session, int idl, int idb, point_t new_pos);

void insert_pax_to_line(session_t session, int idl, point_t pos1, point_t pos2);

void get_random_stops(session_t session, int idl);

void receive_core(package_t data);

void receive_lines(package_t data);

void receive();
