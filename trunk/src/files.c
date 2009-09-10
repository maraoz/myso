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


    if(opdir){
        if(opdir->d_name[0] != '.')
        {
            strcpy(name, "../files");
            name[8]='/';
            strcpy(name+9, opdir->d_name);
    
            fd = open(name, O_RDONLY);

            return fd;
        }
    }
    return 0;
}

int
get_path(int fd, point_t ** path)
{
    int i = 0;
    int num;
    char cant;

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

    lseek(fd, 0, SEEK_SET);
    
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


