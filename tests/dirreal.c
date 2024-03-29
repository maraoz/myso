#include<dirent.h>
// #include<fildes.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../inc/typedef.h"

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
#define ISNUM(a) ( (a) >= '0' && (a) <= '9' )

DIR * direct;

/*
** readtoint recibe un fd y retorna en formato int el 
** numero que estaba en el archivo.
*/

DIR * direct;

/*
** readtoint recibe un fd y retorna en formato int el 
** numero que estaba en el archivo.
*/

int
readToInt(int fd)
{
    char temp;
    int num = 0;

   while( read(fd, &temp, sizeof(char)) )
    {
        if(ISNUM(temp))
            num = num * 10 + temp - '0';
        else
            break;
    }

    return num;
}

int
openFiles(void)
{
    struct dirent *opdir;
    int fd;
    char name[20];


    opdir = readdir(direct);

              
    if(opdir)
    {
        if(ISNUM(opdir->d_name[0]))
        {
            strcpy(name, "../files");
            name[8]='/';
            strcpy(name+9, opdir->d_name);
    
            fd = open(name, O_RDONLY);

            return fd;
        }
        else
            return -1;
    }

    return 0;
}

int 
preparefd(int fd){
    return dup2(fd,3);
}

int
get_path(int fd, point_t ** path)
{
    int i = 0;
    int num;
    char cant;

    lseek(fd, 0, SEEK_SET);

    cant = readToInt(fd);

    *path = malloc(cant * sizeof(point_t));

    while( i < cant )
    {
        num = readToInt(fd);
        (*path)[i].x = num;

        num = readToInt(fd);
            (*path)[i].y = num;

        i++;
    }

    return cant;
}

int
get_qty_buses(int fd)
{
    int cant;

    cant = readToInt(fd);

    return cant;
}

int *
get_times(int fd)
{
    int i = 0;
    int var;
    int * times = NULL;

    do
    {
	    times = realloc(times, (i+1) * sizeof(int));
	    var = readToInt(fd);
	    times[i++] = var;
    } while(var);

    return times;
}

int
get_stops(int fd, point_t ** stops)
{
    int i = 0;
    int cant;
    int num;

    cant = readToInt(fd);

    *stops = malloc(cant * sizeof(point_t));

    while( i < cant )
    {
        num = readToInt(fd);
        (*stops)[i].x = num;
        num = readToInt(fd);
        (*stops)[i].y = num;
	
        i++;
    }

    return cant;
}

int
closeFd(int fd)
{
    return close(fd);
}

void
openDir(void)
{
    char * dir = "../files";
    direct = opendir(dir);

}

void
closeDir(void)
{
    closedir(direct);
}

void
mostrarTodo(int fd)
{
    int temp;

    while( read(fd, &temp, sizeof(char)) && ( temp >= '0' || temp <= '9' ) )
	printf("%c\n", temp);
}

void
ignore()
{
    openFiles();
    openFiles();
    openFiles();
}

int
main(void)
{
    int fds;
    int cant;
    int sale;
    int i;
    point_t * path;
    point_t * stops;
    int * tiempos;

    openDir();

    while((fds = openFiles()) != 0 )
    {
        printf("el fd vale exactamente %d\n", fds);
        if(fds != -1)
        {
            preparefd(fds);

            cant = get_path(fds, &path);
            printf("cantidad de recorridos = %d\n", cant);
            for( i = 0 ; i < cant ; i++ )
                printf("recorrido %d = %d %d\n", i, path[i].x, path[i].y);

            cant = get_qty_buses(fds);
            printf("cantidad de colectivos = %d\n", cant);

            tiempos = get_times(fds);
            for( i = 0 ; tiempos[i] ; i++ )
                printf("tiempos = %d\n", tiempos[i]);

            cant = get_stops(fds, &stops);
            printf("cantidad de paradas = %d\n", cant);
            for( i = 0 ; i < cant ; i++ )
                printf("paradas %d = %d %d\n", i, stops[i].x, stops[i].y);

            closeFd(fds);
            closeFd(3);
           }
    }

    closeDir();
}



