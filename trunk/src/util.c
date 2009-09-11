#include "../inc/typedef.h"
#include <stdlib.h>
#include <math.h>

float
dist(point_t a, point_t b){
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}


int digit_count(int n) {
    int i;
    if (n == 0)
        return 1;
    for (i=0; n != 0; i++) {
        n /= 10;
    }
    return i;
}

char * itoa( int n) {
    char * ret = calloc(11,sizeof(char));
    if (n == 0) {
        ret[0] = '0';
        return ret;
    }
    int i;
    int dc = digit_count(n);
    for (i=0; n != 0; i++) {
        ret[dc-i-1] = n % 10 + '0';
        n /= 10;
    }
    return ret;
}
