#include<dirent.h>
#include<fildes.h>
#include<stdio.h>

/*
** formato del file:
** c 					cantidad de colectivos.
** ac					cantidad de puntos de recorrido.
** a1 b1 a2 b2 a3 c3 .. an bn 		recorrido.
** cc					cantidad de paradas.
** c1 d1 c2 d2 c3 d3 .. cn dn 		paradas.
** tc					cantidad de tiempos.
** t1 t2 t3 ... tn			tiempos de salidas.
*/

#define MAXFILES 10
#define ISNUM(a) ( (a) >= '0' || (a) <= '9' )

typedef struct{
    int cant;
    int fd_lst[MAXFILES];
} fd_strct;

fd_strct fd_list;

/*
** readtoint recibe un fd y retorna en formato int el 
** numero que estaba en el archivo.
*/

int
readToInt(int fd)
{
    char vec;
    int num = 0;
    
    while( ISNUM(read(fd, &vec, sizeof(char))) )
	num = num * 10 + vec - '0';
    
    return num;
}

/*
** openfiles abre todos los archivos cada uno en un file descriptor.
** se lo llama una sola vez. el ciclo hay que hacerlo con returnfd.
** openfiles deja la informacion (cantidad de fd y numero de fd) en una
** estructura global.
*/

int
openFiles(void)
{
    DIR * direct;
    struct dirent *opdir;
    char * dir = "../files";
    int fd_list.cant = 0;

    if( (direct = opendir(dir)) == NULL )
	printf("Error de directorio\n");

    while( (opdir = readdir( direct )) != NULL )
	fd_list[i-1] = open(opdir->d_name, O_RDONLY, 0);

    closedir(direct);
}

/*
** returnfd va devolviendo los fd uno a uno cada vez que es llamada.
*/

int
returnFd()
{
    static int i = 0;
    int fd;
    
    fd = fd_list[i];
    i++;
    
    return fd;
}

/*
** necesito que antes de entrar al for este todo listo
** como para que el proximo caracter que lea sea el
** valor x de la primera posicion del recorrido.
** para eso necesito sacarme las dudas que tengo.
*/


/*
** me falta saber como mover el cursor dentro del file
** descriptor para ir saltando de un lugar a otro.
*/

int
get_path(int fd, point_t ** path)
{
    int i,j;
    int num;
    int cant;

    /* posiciono el cursor en cantidad de puntos*/
    
    cant = readToInt(fd);

    *path = malloc(cant * sizeof(point_t));
    
    /* posiciono el cursor en los puntos */

    for(i=0, j=0; i < cant ; i++)
    {
	num = readToInt(fd);
    
        if(i%2)
            (*path)[j].x = num;
        else
            (*path)[j++].y = num;
    }

    return j;
}

int
get_qty_buses(int fd)
{
    int cant;

    /* posiciono el cursor en cantidad de colectivos */
    
    cant = readToInt(fd);

    return cant;
}

int *
get_times(int fd)
{
    int i;
    int cant;
    int * times;

    /* posiciono el cursor en cantidad de tiempos*/

    cant = readToInt(fd);
    
    times = malloc(cant * sizeof(int));

    /* posiciono el cursor en tiempos */

    for(i=0; i < cant ; i++)
	    times[i] = readToInt(fd);

    return times;
}

int
get_stops(int fd, point_t ** stops)
{
    int i,j;
    int cant;
    int num;

    /* posiciono el cursor en cantidad de paradas*/

    cant = readToInt(fd);

    *stops = malloc(cant * sizeof(point_t));
    
    /* posiciono el cursor en las paradas */

    for(i=0, j=0; i < cant; i++)
    {
	num = readToInt(fd);
	
        if(i%2)
            (*stops)[j].x = num;
        else
            (*stops)[j++].y = num;
    }

    return j;
}

int
closeFd(int fd)
{
    return close(fd);
}
