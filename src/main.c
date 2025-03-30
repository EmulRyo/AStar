#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "int2.h"
#include "astar.h"

enum CellAction_t { NONE, BLOCK, UNBLOCK };
int g_gridCols = 19;
int g_gridRows = 10;
Int2 g_start   = { -1, -1 };
Int2 g_goal    = { -1, -1 };
enum CellAction_t g_cellAction = NONE;
Texture2D g_arrow;

Int2 GetCellID(Int2 screenPos) {
    Int2 cell = { -1, -1 };

    int minScreenSize = min(GetScreenWidth(), GetScreenHeight());
    int margin = (int)(minScreenSize * 0.05f);
    int gridSize = minScreenSize - (2 * margin);
    int cellSize = gridSize / g_gridRows;
    int gridWidth = cellSize * g_gridCols;
    int gridHeight = cellSize * g_gridRows;

    if ((screenPos.x < margin) || (screenPos.y < margin))
        return cell;

    if ((screenPos.x > margin + gridWidth) || (screenPos.y > margin + gridHeight))
        return cell;

    cell.x = (screenPos.x - margin) / cellSize;
    cell.y = (screenPos.y - margin) / cellSize;

    return cell;
}

float GetArrowRotation(Int2* path, size_t pathLen, int i, Int2 goal) {
    float rotation = 0;
    Int2* p = &path[i];
    Int2 dir = { 0 };
    if (i < (pathLen - 1)) {
        Int2* q = &path[i + 1];
        dir = Int2Sub(*q, *p);
    }
    else {
        dir = Int2Sub(goal, *p);
    }
    
    if (dir.x > 0)
        rotation = 0;
    else if (dir.x < 0)
        rotation = 180;
    else if (dir.y > 0)
        rotation = 90;
    else if (dir.y < 0)
        rotation = 270;

    return rotation;
}

void Update() {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

        Int2 cell = GetCellID((Int2){ GetMouseX(), GetMouseY() });

        if (Int2IsValid(cell)) {
            bool modified = false;
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (!Int2IsValid(g_start)) {
                    g_start = cell;
                    AStarSet(cell.x, cell.y, false);
                    modified = true;
                }

                if (!modified && !Int2IsValid(g_goal) && (!Int2Equals(cell, g_start))) {
                    g_goal = cell;
                    AStarSet(cell.x, cell.y, false);
                    modified = true;
                }

                if (!modified && Int2Equals(g_start, cell)) {
                    g_start = (Int2){ -1, -1 };
                    modified = true;
                }

                if (!modified && Int2Equals(g_goal, cell)) {
                    g_goal = (Int2){ -1, -1 };
                    modified = true;
                }

                if (!modified) {
                    bool blocked = AStarGet(cell.x, cell.y);
                    g_cellAction = blocked ? UNBLOCK : BLOCK;
                    AStarSet(cell.x, cell.y, !blocked);
                    modified = true;
                }
            }
            else {
                if (g_cellAction != NONE) {
                    bool blocked = g_cellAction == BLOCK ? true : false;
                    AStarSet(cell.x, cell.y, blocked);
                }
            }
        }

        if (Int2IsValid(g_start) && Int2IsValid(g_goal))
            AStarSearch(g_start, g_goal);
    }

    if (IsMouseButtonUp(MOUSE_BUTTON_LEFT))
        g_cellAction = NONE;
}

void DrawPath(int margin, int cellSize) {
    if (Int2IsValid(g_start) && Int2IsValid(g_goal)) {
        Int2* path = NULL;
        size_t pathLen = 0;
        AStarPath(&path, &pathLen);
        for (int i = 0; i < pathLen; i++) {
            Int2* p = &path[i];
            float rotation = GetArrowRotation(path, pathLen, i, g_goal);
            DrawTexturePro(
                g_arrow,
                (Rectangle) {
                0, 0, (float)g_arrow.width, (float)g_arrow.height
            },
                (Rectangle) {
                margin + (cellSize * p->x) + cellSize * 0.5f, margin + (cellSize * p->y) + cellSize * 0.5f, cellSize * 0.5f, cellSize * 0.5f
            },
                (Vector2) {
                cellSize * 0.25f, cellSize * 0.25f
            },
                rotation,
                ColorAlpha(WHITE, 0.5f));
        }
    }
}

void Draw() {
    int minScreenSize = min(GetScreenWidth(), GetScreenHeight());
    int margin = (int)(minScreenSize *0.05f);
    int gridSize = minScreenSize - (2 * margin);
    int cellSize = gridSize / g_gridRows;
    int gridWidth = cellSize * g_gridCols;
    int gridHeight = cellSize * g_gridRows;

    int xOffset = margin;
    for (int col = 0; col < g_gridCols+1; col++) {
        DrawLine(xOffset, margin, xOffset, margin + gridHeight, WHITE);
        xOffset += cellSize;
    }

    int yOffset = margin;
    for (int row = 0; row < g_gridRows+1; row++) {
        DrawLine(margin, yOffset, margin + gridWidth, yOffset, WHITE);
        yOffset += cellSize;
    }

    if (Int2IsValid(g_start))
        DrawRectangle(margin + (cellSize * g_start.x), margin + (cellSize * g_start.y), cellSize-1, cellSize-1, RED);

    if (Int2IsValid(g_goal))
        DrawRectangle(margin + (cellSize * g_goal.x), margin + (cellSize * g_goal.y), cellSize-1, cellSize-1, GREEN);

    for (int row = 0; row < g_gridRows; row++) {
        for (int col = 0; col < g_gridCols; col++) {
            if (AStarGet(col, row))
                DrawRectangle(margin + (cellSize * col), margin + (cellSize * row), cellSize - 1, cellSize - 1, LIGHTGRAY);
        }
    }

    DrawPath(margin, cellSize);
}

void main() {
    AStarInit(g_gridCols, g_gridRows);

    SetWindowState(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "AStar");

    g_arrow = LoadTexture("resources/white-arrow.png");

    while (!WindowShouldClose()) {
        Update();

        BeginDrawing();

        ClearBackground((Color) {28, 28, 28, 255});
        Draw();
        const char* t = TextFormat("A*: %d", GetFPS());
        DrawText(t, 10, 10, 20, WHITE);

        EndDrawing();
    }
    CloseWindow();
    AStarDestroy();
}