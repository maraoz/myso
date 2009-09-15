#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include "../inc/typedef.h"
#include "../inc/core.h"
#include "../inc/draw.h"

extern boolean tiles[YDIM][XDIM];
extern semaphore semps[(CUADRAS+1)*(CUADRAS+1)-4];
extern int semps_hash[(CUADRAS+1)][(CUADRAS+1)];
extern pthread_mutex_t semaphore_mutex;
extern pthread_mutex_t map_mutex;
extern int sim_on;


void print_square(point_t point);
void print_all_squares(void);
void print_bus(point_t point);
void print_h_sem(point_t point);
void print_v_sem(point_t point);


int Xpositions[19] = {2,6,11,15,19,24,28,32,37,41,45,50,54,58,63,67,71,76,80};
int Ypositions[19] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36};
WINDOW *city_win, *log_win, *city_box, *log_box;

WINDOW* create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW * local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}


void *
draw(void)
{
    int i,j;
    point_t aux;
	int startx, starty, width, height;

	initscr();			/* Start curses mode 		*/
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

	height = 45;
	width = 90;
	starty = (LINES - height) / 2;	/* Calculating for a center placement */
	startx = 0;	/* of the window		*/
	printw("Press F1 to exit");
	refresh();
	city_box = create_newwin(height, width, starty, startx);
	log_box = create_newwin(height, width-30, starty, startx+width+5);
	city_win = subwin(city_box,height-4, width-5, starty+2, startx+3);
	log_win = subwin(log_box,height-2, width-32, starty+1, startx+width+6);
	scrollok(log_win, TRUE);

	while(sim_on) {

	int x,y;
	wrefresh(city_win);
	wrefresh(log_win);
	getyx(log_win,y,x);
	werase(city_win);
	
        usleep(10000);
        pthread_mutex_lock(&map_mutex);

        if (has_colors())
            start_color();
       

        print_all_squares();

        for(i = 0; i < YDIM; i++)
            for(j = 0; j < XDIM; j++)
                if( !((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) || 
		 !((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2)))
                {
		    aux.x = j;
		    aux.y = i;
                    if(hasSemaphore(aux))
		    {
			aux.x = Xpositions[j];
			aux.y = Ypositions[i];
			
                        if(isVRedHGreen(semps[(semps_hash[i][j])]))
                            print_v_sem(aux);
                        else
                            print_h_sem(aux);
		    }
                    if(tiles[i][j])
		    {
			aux.x = Xpositions[j];
			aux.y = Ypositions[i];
                        print_bus(aux);
		    }

		    refresh(); 

		}

		pthread_mutex_unlock(&map_mutex);
        }
		
	endwin();			/* End curses mode		  */
	pthread_exit(0);
}


void
print_square(point_t point)
{
    int i;

    for(i = 0; i < 4; i++)
        mvwprintw(city_win,(point.y)+i, point.x, "|||||||||");
}

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

void
print_bus(point_t point)
{
    mvwprintw(city_win,point.y, point.x, "##");
    mvwprintw(city_win,(point.y)+1, point.x, "##");
}

void
print_h_sem(point_t point)
{
    mvwprintw(city_win,point.y, (point.x)-2, "|");
    mvwprintw(city_win,(point.y)+1, (point.x)-2, "|");
    mvwprintw(city_win,point.y, (point.x)+3, "|");
    mvwprintw(city_win,(point.y)+1, (point.x)+3, "|");
}

void
print_v_sem(point_t point)
{
    mvwprintw(city_win,point.y, (point.x)-2, "-");
    mvwprintw(city_win,(point.y)+1, (point.x)-2, "-");
    mvwprintw(city_win,point.y, (point.x)+3, "-");
    mvwprintw(city_win,(point.y)+1, (point.x)+3, "-");
}
