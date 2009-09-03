#include "../inc/typedef.h"
#include include <dirent.h>

#define CD_NO 0
#define CD_INSERT_BUS 1
#define CD_MOVE_BUS

typedef struct
{
    int cantFiles;
    session_t * filesId;
} files_t;

void
insert_request(int * id, point_t pos)
{
    package_t data;

    data.msg_id = CD_INSERT_BUS;
    data.integer_p = id;
    data.point = pos;
    
    w_write(data);
}

void
move_request(int id, point_t new_pos)
{
    package_t data;

    data.msg_id = CD_MOVE_BUS;
    data.integer_p = id;
    data.point = new_pos;

    send(data);
}

void
receive(package_t data)
{
    switch(data.msg_id)
    {
	case CD_INSERT_BUS: insert_bus(data.integer_p, data.point);
	case CD_MOVE_BUS: move_bus*

}


/*
** openFiles recibe un char* con el directorio y retorna un puntero a files_t en el cual 
** contiene un int con la cantidad de archivos que abrio (porque los abre)
** y un string de session_t con todos los fd de los archivos abiertos que devolvio w_open
*/

files_t *
openFiles(char * dir)
{
    DIR * direct;
    struct dirent *opdir;
    files_t * files = malloc(sizeof(files_t));
    int i = 0;
    int fd;

    if( (direct = opendir(dir)) == NULL )
	printf("Error de directorio\n");

    files->cantFiles = 0;
    files->filesId = NULL;

    while( (opdir = readdir( direct )) != NULL )
    {
	fd = w_open( opdir->d_name );
	
	files->cantFiles++;
	
	if(i%5==0)
	    files->filesId=realloc((i+5)*sizeof(int));
	
	files->filesId[i] = fd;
    }

    files->filesId=realloc(i*sizeof(int));
    
    return files;
}





