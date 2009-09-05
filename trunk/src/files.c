#include<dirent.h>
#include<stdio.h>

/*
** formato del file:
** c * a1 b1 a2 b2 a3 c3 .. an bn * c1 d1 c2 d2 c3 d3 .. cn dn * t1 t2 t3 ... tn *
** c = cantidad de colectivos.
** despues viene el recorrido.
** despues vienen las paradas.
** t = tiempos.
*/

/*
** lo que voy a hacer es dejar todo listo la informacion pedida en una 
** estructura que despeus vamos a hablar con matias de que forma se la
** paso, si puedo retornar habiendo hecho malloc de ellas, o las
** tenemos que transferir con las funciones para conmunicacion entre
** procesos (eso no lo se).
** La causa del cambio y de que no lo voy a hacer como habiamos dicho, es
** que para devolver los fd de las lineas, si me llama muchas veces a mi
** funcion filesOpen, tengo que abrir el directorio cada vez que me llama
** y cerrarlo, lo que me parece bastante berreta hacer eso.
** se podria solucionar haciendo variables globales pero no me parece
** tampoco la mejor solucion.
*/


/*
preguntas: 

1) asumo entonces que puedo hacer un malloc y devolverlo por parametro
entonces por que tuvimos que hacer ese cambio en el openFiles para que el 
while lo haga wilson en vez de hacerlo yo dentro de la funcion y devolver una 
estructura, yo creo que era porque no se podia devolver una estructura.

2) por que se supone que cuando llego a get_path() el archivo ya esta abierto?
no deberia llamar antes a alguna funcion tipo openFiles??

3) despues de correr get_path() donde queda el cursor dentro del archivo?
cuando entro a get_stops() lo tengo al principio del archivo o en lo ultimo que lei
*/

/*
** A esta funcion hay que arreglarla, despues hablare con matias como lo hacemos.
*/

int
openFiles(char * dir)
{
    DIR * direct;
    struct dirent *opdir;
    int i = 0;
    int fd;

    if( (direct = opendir(dir)) == NULL )
	printf("Error de directorio\n");

    while( (opdir = readdir( direct )) != NULL )
	printf("file nombre = %s\n", opdir->d_name);

	closedir(direct);
}

/*
** necesito que antes de entrar al for este todo listo
** como para que el proximo caracter que lea sea el
** valor x de la primera posicion del recorrido.
** para eso necesito sacarme las dudas que tengo.
*/

int
get_path(int fd, int ** path)
{
    int i,j;
    char num;
    char * param = malloc(3);
    param = itoa(fd);

    *path = NULL;

    for(i=0, j=0; (num = fgetc(param)) != '*'; i++)
    {
        if(i%3 == 0)
        {
            *path = realloc((j+1)*sizeof(point_t));
            (*path)[j].x = (int)num;
        }
        else if(i%3 == 1)
            (*path)[j].y = (int)num;
    }

    free(param);

    return j;
}

int
get_qty_buses(int fd)
{
    char * param = malloc(3);
    param = itoa(fd);
    char num;

    num = fgetc(param);

    free(param);

    return (int)num;
}

int *
get_times(fd)
{
    char * param = malloc(3);
    param = itoa(fd);
    int i;
    int * times;
    char num;

    for(i=0; (num = fgetc(param)) != '*'; i++)
        if(i%2 == 0)
        {
            times = realloc((i+1)*sizeof(int));
            times[i] = (int)num;
        }

    free(param);

    return times;
}

int
get_stops(int fd, int ** stops)
{
    int i,j;
    char num;
    char * param = malloc(3);
    param = itoa(fd);

    *stops = NULL;

    for(i=0, j=0; (num = fgetc(param)) != '*'; i++)
    {
        if(i%3 == 0)
        {
            *stops = realloc((j+1)*sizeof(point_t));
            (*stops)[j].x = (int)num;
        }
        else if(i%3 == 1)
            (*stops)[j].y = (int)num;
    }

    free(param);

    return j;
}

int
closeFd(int fd)
{
    char * param = malloc(3);
    param = itoa(fd);

    return fclose(param);
}
