#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "../inc/typedef.h"
#include "../inc/core.h"
#include "../inc/draw.h"

extern boolean tiles[YDIM][XDIM];
extern semaphore semps[(CUADRAS+1)*(CUADRAS+1)-4];
extern int semps_hash[(CUADRAS+1)][(CUADRAS+1)];
extern pthread_mutex_t semaphore_mutex;
extern pthread_mutex_t map_mutex;
extern int sim_on;
extern point_t buses[XDIM*YDIM][XDIM*YDIM];

void print_square(point_t point);
void print_all_squares(void);
void print_bus(point_t point1, point_t point2);
void print_h_sem(point_t point);
void print_v_sem(point_t point);


int Xpositions[19] = {2,6,11,15,19,24,28,32,37,41,45,50,54,58,63,67,71,76,80};
int Ypositions[19] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36};

WINDOW *city_win, *log_win, *city_box, *log_box;

WINDOW* create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);
	wrefresh(local_win);
	return local_win;
}

void *
draw(void)
{
    WINDOW * hola;
    int i,j;
    point_t aux, aux2;
	int startx, starty, width, height;

	initscr();			
	cbreak();
	keypad(stdscr, TRUE);
	height = LINES-2;
	width = (COLS+25) / 2;
	starty = 2;	
	startx = 0;	
	printw("Press Q to exit");
	refresh();
	city_box = create_newwin(height, width, starty, startx);
    log_box = create_newwin(height, width-30, starty, startx+width+5);
    werase(city_box);
    werase(log_box);
    city_win = newwin(height-4, width-5, starty+2, startx+3);
    log_win = newwin(height-2, width-32, starty+1, startx+width+6);
	scrollok(log_win, TRUE);

	while(sim_on) {

	int x,y;
	wrefresh(city_win);
	wrefresh(log_win);
	werase(city_win);
	
        usleep(100000);
        pthread_mutex_lock(&map_mutex);

        if (has_colors())
            start_color();
       

        print_all_squares();

        for(i = 0; i < YDIM; i++)
            for(j = 0; j < XDIM; j++)
                if( !((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) || 
		 !((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2)))
                {
		    aux2.x = j;
		    aux2.y = i;
                    aux.x = Xpositions[j];
                    aux.y = Ypositions[i];

                    if(hasSemaphore(aux2))
		    {
                        if(isVRedHGreen(semps[(semps_hash[i][j])]))
                            print_v_sem(aux);
                        else
                            print_h_sem(aux);
		    }
                    if(tiles[i][j])
                        print_bus(aux, aux2);

		    refresh(); 

		}

		pthread_mutex_unlock(&map_mutex);
        }
		
    delwin(city_box);
    delwin(city_win);
    delwin(log_box);
    delwin(log_win);
    endwin();	
    pthread_exit(0);
}

/* Dibuja una cuadra */
void
print_square(point_t point)
{
    int i;

    for(i = 0; i < 4; i++)
        mvwprintw(city_win,(point.y)+i, point.x, "|||||||||");
}

/* Dibuja las cuadras */
void
print_all_squares(void)
{
    int i,j;
    point_t point;

    for( i = 2; i < 37; i += 6 )
        for( j = 5 ; j < 81; j += 13 )
        {
            point.x = j;
            point.y = i;
            print_square(point);
        }
}

/* Printea los colectivos, indicando numero de linea y id
 * del colectivo */
void
print_bus(point_t point1, point_t point2)
{
    int i, j, flag = 1;

    for(i = 0; i < XDIM*YDIM && flag; i++)
        for(j = 0; j < XDIM*YDIM && flag; j++)
            if(buses[i][j].x == point2.x && buses[i][j].y == point2.y)
                flag = 0;

    mvwprintw(city_win,point1.y, point1.x, "%02d", i);
    mvwprintw(city_win,(point1.y)+1, point1.x, "%02d", j);
}

/* Printea los semaforos horizontales */
void
print_h_sem(point_t point)
{
    mvwprintw(city_win,point.y, (point.x)-2, "|");
    mvwprintw(city_win,(point.y)+1, (point.x)-2, "|");
    mvwprintw(city_win,point.y, (point.x)+3, "|");
    mvwprintw(city_win,(point.y)+1, (point.x)+3, "|");
}

/* Printea los semaforos verticales */
void
print_v_sem(point_t point)
{
    mvwprintw(city_win,point.y, (point.x)-2, "-");
    mvwprintw(city_win,(point.y)+1, (point.x)-2, "-");
    mvwprintw(city_win,point.y, (point.x)+3, "-");
    mvwprintw(city_win,(point.y)+1, (point.x)+3, "-");
}
