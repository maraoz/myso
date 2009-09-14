#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include "../inc/typedef.h"
#include "../inc/core.h"

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


int Xpositions[19] = {1,5,10,14,18,23,27,31,36,40,44,49,53,57,62,66,70,75,79};
int Ypositions[19] = {0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36};


void *
draw(void)
{
    int i, j;
    point_t aux;

    while(sim_on) 
    {
        usleep(10000);
        pthread_mutex_lock(&map_mutex);

       initscr();

        if (has_colors())
            start_color();

        erase();

        print_all_squares();

        for(i = 0; i < 19; i++)
            for(j = 0; j < 19; j++)
                if( !((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) 
                    || !((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2)))
                {
//                     sleep(1);
                    aux.x = Xpositions[i];
                    aux.y = Ypositions[j];
                    if(hasSemaphore(aux))
                        if(isVRedHGreen(semps[(semps_hash[i][j])]))
                            print_v_sem(aux);
                        else
                            print_h_sem(aux);
                    else if(tiles[i][j])
                        print_bus(aux);
                    refresh();
               }
        }

        pthread_mutex_unlock(&map_mutex);

        endwin();
        return;
}

// {
//     int i,j;
//     while(sim_on) {
//         usleep(10000);
//         pthread_mutex_lock(&map_mutex);
//         
//         erase();
//         for(i = 0 ; i < YDIM ; i++){
//             for( j = 0 ; j < XDIM ; j++) {
//                 if(((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) 
//                 && ((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2))) {
//                     mvprintw(i, j, "+");
//                 } else {
//                     point_t aux;
//                     aux.x = j;
//                     aux.y = i;
//                     if(hasSemaphore(aux)) {
//                         if(isVRedHGreen(semps[(semps_hash[i][j])])){
//                             mvprintw(i, j, "-",tiles[i][j]?'c':' ');
//                         } else {
//                             mvprintw(i, j, "|",tiles[i][j]?'c':' ');
//                         }
//                     } else{
//                         mvprintw(i, j, "%c",tiles[i][j]?'c':' ');
//                     }
//                 }
//             }
//         }
//         refresh();
//         //endwin();
//         pthread_mutex_unlock(&map_mutex);
//         
//     }
// }



void
print_square(point_t point)
{
    int i;

    for(i = 0; i < 4; i++)
        mvprintw((point.y)+i, point.x, "|||||||||");
}

void
print_all_squares(void)
{
    int i,j;
    point_t point;

    for( i = 2; i < 37; i += 6 )
        for( j = 4 ; j < 81; j += 13 )
        {
            point.x = j;
            point.y = i;
            print_square(point);
        }
}

void
print_bus(point_t point)
{
    mvprintw(point.y, point.x, "##");
    mvprintw((point.y)+1, point.x, "##");
}

void
print_h_sem(point_t point)
{
    mvprintw(point.y, (point.x)-2, "|");
    mvprintw((point.y)+1, (point.x)-2, "|");
    mvprintw(point.y, (point.x)+3, "|");
    mvprintw((point.y)+1, (point.x)+3, "|");
}

void
print_v_sem(point_t point)
{
    mvprintw(point.y, (point.x)-2, "-");
    mvprintw((point.y)+1, (point.x)-2, "-");
    mvprintw(point.y, (point.x)+3, "-");
    mvprintw((point.y)+1, (point.x)+3, "-");
}
