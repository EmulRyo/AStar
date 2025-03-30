#include "int2.h"

bool Int2IsValid(Int2 p) {
    return (p.x >= 0 && p.y >= 0);
}

bool Int2Equals(Int2 a, Int2 b) {
    return (a.x == b.x && a.y == b.y);
}

Int2 Int2Add(Int2 a, Int2 b) {
    return (Int2) { a.x + b.x, a.y + b.y };
}

Int2 Int2Sub(Int2 a, Int2 b) {
    return (Int2) { a.x - b.x, a.y - b.y };
}

Int2 Int2AddS(Int2 v, int x, int y) {
    return (Int2) { v.x + x, v.y + y };
}

Int2 Int2SubS(Int2 v, int x, int y) {
    return (Int2) { v.x - x, v.y - y };
}
