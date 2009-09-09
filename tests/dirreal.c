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
    int temp;
    int num = 0;

    while( read(fd, &temp, sizeof(char)) && ISNUM(temp) )
	num = num * 10 + temp - '0';
    
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
	if(opdir->d_name[0]!='.')
	{
	    strcpy(name, "../files");
	    name[8]='/';
	    strcpy(name+9, opdir->d_name);

	    fd = open(name, O_RDONLY);
	    return fd;
	}

    return 0;
}

int
get_path(int fd, point_t ** path)
{
    int i,j;
    int num;
    char cant;

    /* posiciono el cursor en cantidad de puntos*/
    lseek(fd, 0, SEEK_SET);
    
    cant = readToInt(fd);

    *path = malloc(cant * sizeof(point_t));
    
    /* posiciono el cursor en los puntos */
    lseek(fd, sizeof('\n'), SEEK_CUR);

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


int
main(void)
{
    int fds;
    int cant;
    int sale;
    int i;

    openDir();

    openFiles();
    openFiles();

    fds = openFiles();

   mostrarTodo(fds);

//     sale = lseek(fds, 0, SEEK_SET);
//     cant = readToInt(fds);
//     printf("salio con %d\n", sale);
//     printf("%d\n", cant);
    
    for( i = 0 ; i<100 ; i++)
    {
	sale = lseek(fds, i, SEEK_SET);
	cant = readToInt(fds);
	printf("salio con %d\n", sale);
	printf("%d\n", cant);
    }
/*    
    sale = lseek(fds, 0, SEEK_SET);
    cant = readToInt(fds);
    printf("salio con %d\n", sale);
    printf("%d\n", cant);*/
    
    closeFd(fds);

    closeDir();
}



