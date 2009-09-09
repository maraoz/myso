int openFiles(void);

int get_path(int fd, point_t ** path);

int get_qty_buses(int fd);

int * get_times(int fd);

int get_stops(int fd, point_t ** stops);

int closeFd(int fd);

void closeDir(void);