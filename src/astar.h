#ifndef __ASTAR_H__
#define __ASTAR_H__

#include "int2.h"

enum NavigationMode { FOUR_SIDES, EIGHT_SIDES };

void AStarInit(size_t width, size_t height, enum NavigationMode navMode);
void AStarDestroy();
void AStarSetBlocked(size_t x, size_t y, bool blocked);
bool AStarIsBlocked(size_t x, size_t y);
bool AStarIsVisited(size_t x, size_t y);
bool AStarSearch(Int2 start, Int2 goal);
void AStarPath(Int2** path, size_t* pathLen);

#endif