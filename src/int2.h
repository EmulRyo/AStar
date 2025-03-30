#ifndef __INT2_H__
#define __INT2_H__

#include <stdbool.h>

typedef struct Int2 {
    int x, y;
} Int2;

bool Int2IsValid(Int2 p);
bool Int2Equals(Int2 a, Int2 b);
Int2 Int2Add(Int2 a, Int2 b);
Int2 Int2Sub(Int2 a, Int2 b);
Int2 Int2AddS(Int2 v, int x, int y);
Int2 Int2SubS(Int2 v, int x, int y);

#endif