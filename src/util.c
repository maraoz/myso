#include "../inc/typedef.h"
#include <math.h>

float
dist(point_t a, point_t b){
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}
