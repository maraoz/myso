#include <ncurses.h>
#include <unistd.h>
#include "../inc/typedef"

void *
draw(void) {
    int i,j;
    while(sim_on) {
        usleep(1000);
        pthread_mutex_lock(&map_mutex);
        initscr();
        for(i = 0 ; i < YDIM ; i++){
            for( j = 0 ; j < XDIM ; j++) {
                if(((i%(TILES_CUADRAS+1) == 1) || (i%(TILES_CUADRAS+1) == 2)) 
                && ((j%(TILES_CUADRAS+1) == 1) || (j%(TILES_CUADRAS+1) == 2))) {
                    mvprintw(j, i, "+");
                } else {
                    point_t aux;
                    aux.x = j;
                    aux.y = i;
                    if(hasSempahore(aux)) {
                        if(isVRedHGreen(semps[(semps_hash[i][j])])){
                            mvprintw(j, i, "-",tiles[i][j]?'c':' ');
                        } else {
                            mvprintw(j, i, "|",tiles[i][j]?'c':' ');
                        }
                    } else {
                        mvprintw(j, i, "%c",tiles[i][j]?'c':' ');
                    }
                }
            }
        }
        refresh();
        endwin();
        pthread_mutex_unlock(&map_mutex);
        
    }
}