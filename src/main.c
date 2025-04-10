#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#include <rlgl.h>
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

        double duration = pathLen * 0.5;
        double now = GetTime();
        double wait = 1.0;
        double timeNormalized = fmod(now, duration);
        timeNormalized = timeNormalized / duration;
        double time = timeNormalized * (pathLen+2) - 1.0f;

        for (int i = 0; i < pathLen; i++) {
            Int2* p = &path[i];
            float rotation = GetArrowRotation(path, pathLen, i, g_goal);
            float alpha = 0.1f;
            float dif = fabsf(time - i);
            if (dif < 1.5f) {
                dif = dif / 1.5f;
                alpha = sin((1.0f - dif) * PI/2.0f) * 0.9f + 0.1f;
            }

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
                ColorAlpha(WHITE, alpha));
        }
    }
}

void DrawGridRectangle(Int2 pos, int margin, int cellSize, bool scaleAnim, float hue, float saturation) {
    rlPushMatrix();
    
    float scale = 1.0f;
    if (scaleAnim)
        scale = 1.0f + sin(GetTime()) * 0.1f;

    rlTranslatef(margin + (cellSize * pos.x) + cellSize * 0.5f + 2, margin + (cellSize * pos.y) + cellSize * 0.5f + 2, 0);
    rlScalef(scale, scale, 1.0f);

    Rectangle rectBG = { -cellSize * 0.5f+4, -cellSize * 0.5f+4, (float)(cellSize - 9), (float)(cellSize - 9) };
    DrawRectangleRounded(rectBG, 0.5f, 4, GRAY);

    Rectangle rectFG1 = { -cellSize*0.5f, -cellSize*0.5f, cellSize - 7, cellSize - 7 };
    DrawRectangleRounded(rectFG1, 0.5f, 4, ColorFromHSV(hue, saturation, 1.0f));

    Rectangle rectFG2 = { -cellSize*0.5f+4, -cellSize*0.5f+4, cellSize - 15, cellSize - 15 };
    DrawRectangleRounded(rectFG2, 0.5f, 4, ColorFromHSV(hue, saturation, 0.8f));

    rlPopMatrix();
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
        DrawGridRectangle(g_start, margin, cellSize, true, 0.0f, 1.0f);

    if (Int2IsValid(g_goal))
        DrawGridRectangle(g_goal, margin, cellSize, true, 120.0f, 1.0f);

    for (int row = 0; row < g_gridRows; row++) {
        for (int col = 0; col < g_gridCols; col++) {
            if (AStarGet(col, row))
                DrawGridRectangle((Int2){col, row}, margin, cellSize, false, 0.0f, 0.0f);
        }
    }

    DrawPath(margin, cellSize);
}

void main() {
    AStarInit(g_gridCols, g_gridRows);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
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