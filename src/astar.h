#ifndef __ASTAR_H__
#define __ASTAR_H__

#include "int2.h"

void AStarInit(size_t width, size_t height);
void AStarDestroy();
void AStarSet(size_t x, size_t y, bool blocked);
bool AStarGet(size_t x, size_t y);
bool AStarSearch(Int2 start, Int2 goal);
void AStarPath(Int2** path, size_t* pathLen);

#endif