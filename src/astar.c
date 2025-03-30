#include <stdio.h>
#include <float.h>
#include <raylib.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#include "astar.h"

typedef struct Node {
    Int2 pos;
    Int2 parent;
    bool blocked;
    // f = g + h
    float f, g, h;
} Node;

typedef struct Grid {
    Node* cells;
    size_t rows;
    size_t cols;
} Grid;

static Grid g_grid = { .cells=NULL, .rows=0, .cols=0 };
static Int2* g_path = NULL;

Node NodeNew(int value) {
    return (Node) {
        .pos    = { value, value },
        .parent = { value, value },
        .blocked = false,
        .f = FLT_MAX,
        .g = FLT_MAX,
        .h = FLT_MAX
    };
}

void NodeInit(Node* node, int value) {
    node->pos     = (Int2){ value, value };
    node->parent  = (Int2){ value, value };
    node->blocked = false;
    node->f       = FLT_MAX;
    node->g       = FLT_MAX;
    node->h       = FLT_MAX;
}

void GridReset(bool setblocked) {
    for (int i = 0; i < g_grid.cols * g_grid.rows; i++) {
        if (setblocked)
            g_grid.cells[i].blocked = false;
        g_grid.cells[i].parent = (Int2){ -1, -1 };
        g_grid.cells[i].f = FLT_MAX;
        g_grid.cells[i].g = FLT_MAX;
        g_grid.cells[i].h = FLT_MAX;
    }
}

Node* GridNodeGet(Int2 pos) {
    size_t offset = pos.y * g_grid.cols + pos.x;
    return &(g_grid.cells[offset]);
}

void ListNodeInsertSorted(Node** list, Node* node) {
    ptrdiff_t id = 0;
    ptrdiff_t len = arrlen(*list);
    for (ptrdiff_t i = len-1; i >= 0; i--) {
        if (node->f <= (*list)[i].f) {
            id = i + 1;
            break;
        }
    }

    Node n = *node;
    arrins(*list, id, n);
}

int ListNodeGetPosIndex(Node* list, Int2 pos) {
    for (int i = 0; i < arrlen(list); i++) {
        if (Int2Equals(list[i].pos, pos))
            return i;
    }
    return -1;
}

bool PosIsInside(Int2 pos) {
    return pos.x >= 0 && pos.y >= 0 && pos.x < g_grid.cols && pos.y < g_grid.rows;
}

bool PosIsBlocked(Int2 pos) {
    return g_grid.cells[pos.y*g_grid.cols + pos.x].blocked == true;
}

bool PosIsValid(Int2 pos) {
    return PosIsInside(pos) && !PosIsBlocked(pos);
}

int ManhattanDistance(Int2 a, Int2 b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

void TracePath(Int2 src, Int2 dst) {
    Node* n = GridNodeGet(dst);
    n = GridNodeGet(n->parent);

    while (!Int2Equals(n->pos, src)) {
        //printf("TracePath: (%d, %d)\n", n->pos.x, n->pos.y);
        arrins(g_path, 0, n->pos);
        n = GridNodeGet(n->parent);
    };
}

void AStarInit(size_t width, size_t height) {
    if (g_grid.cells != NULL)
        free(g_grid.cells);

    g_grid.cells = malloc(width * height * sizeof(Node));
    g_grid.cols = width;
    g_grid.rows = height;
    GridReset(true);
}

void AStarDestroy() {
    if (g_grid.cells != NULL)
        free(g_grid.cells);
}

void AStarSet(size_t x, size_t y, bool blocked) {
    size_t offset = g_grid.cols * y + x;
    g_grid.cells[offset].blocked = blocked;
}

bool AStarGet(size_t x, size_t y) {
    size_t offset = g_grid.cols * y + x;
    return g_grid.cells[offset].blocked;
}

// A* finds a path from start to goal.
bool AStarSearch(Int2 start, Int2 goal) {
    Node* openSet = NULL;

    GridReset(false);
    if (g_path != NULL) {
        arrfree(g_path);
        g_path = NULL;
    }

    Node nStart = NodeNew(-1);
    nStart.pos = start;
    nStart.g = 0.0f;
    nStart.h = (float)ManhattanDistance(start, goal);
    nStart.f = nStart.g + nStart.h;
    arrput(openSet, nStart);

    while (arrlen(openSet) > 0) {
        Node q = arrpop(openSet);

        Int2 s[4] = {
            Int2AddS(q.pos,  1,  0),
            Int2AddS(q.pos,  0,  1),
            Int2AddS(q.pos, -1,  0),
            Int2AddS(q.pos,  0, -1),
        };

        for (int i = 0; i < 4; i++) {
            if (Int2Equals(s[i], goal)) {
                Node* q2 = GridNodeGet(q.pos);
                *q2 = q;
                Node* q3 = GridNodeGet(s[i]);
                q3->pos = s[i];
                q3->parent = q.pos;
                TracePath(start, goal);
                return true;
            }

            if (PosIsValid(s[i])) {
                Node n = { 0 };
                n.pos = s[i];
                n.parent = q.pos;
                n.g = q.g + 1.0f;
                n.h = (float)ManhattanDistance(s[i], goal);
                n.f = n.g + n.h;

                int id = -1;
                id = ListNodeGetPosIndex(openSet, s[i]);
                if ((id >= 0) && (openSet[id].f <= n.f))
                    continue;

                Node* q2 = GridNodeGet(s[i]);
                if (q2->f <= n.f)
                    continue;

                ListNodeInsertSorted(&openSet, &n);
            }
        }

        Node* q3 = GridNodeGet(q.pos);
        if (q.f < q3->f)
            *q3 = q;
    }

    // Open set is empty but goal was never reached
    return false;
}

void AStarPath(Int2** path, size_t* pathLen) {
    *path = g_path;
    *pathLen = arrlenu(g_path);
}
