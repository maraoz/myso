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


void *
draw(void) {
    int i,j;
    while(sim_on) {
        usleep(10000);
        pthread_mutex_lock(&map_mutex);
        
        erase();
        for(i = 0 ; i < YDIM ; i++){
            for( j = 0 ; j < XDIM ; j++) {
                if(((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) 
                && ((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2))) {
                    mvprintw(i, j, "+");
                } else {
                    point_t aux;
                    aux.x = j;
                    aux.y = i;
                    if(hasSemaphore(aux)) {
                        if(isVRedHGreen(semps[(semps_hash[i][j])])){
                            mvprintw(i, j, "-",tiles[i][j]?'c':' ');
                        } else {
                            mvprintw(i, j, "|",tiles[i][j]?'c':' ');
                        }
                    } else{
                        mvprintw(i, j, "%c",tiles[i][j]?'c':' ');
                    }
                }
            }
        }
        refresh();
        //endwin();
        pthread_mutex_unlock(&map_mutex);
        
    }
}
