#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#include <rlgl.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "int2.h"
#include "astar.h"

enum CellAction_t { NONE, BLOCK, UNBLOCK };
int g_gridCols = 19 * 3;
int g_gridRows = 10 * 3;
enum NavigationMode g_gridNavMode = FOUR_SIDES;
int g_gridColsOld = -1;
int g_gridRowsOld = -1;
enum NavigationMode g_gridNavModeOld = FOUR_SIDES;
Rectangle g_gridBoundary;
Int2 g_start = { -1, -1 };
Int2 g_goal = { -1, -1 };
enum CellAction_t g_cellAction = NONE;
Texture2D g_arrow;

bool g_editModeNavMode = false;

#include <time.h>

static double Now()
{
    struct timespec now;
    if (!timespec_get(&now, TIME_UTC))
        printf("Error: timespec_get failed\n");
    return now.tv_sec + now.tv_nsec * 1e-9;
}

static void GetGridData(Rectangle boundary, float* cellSize, Vector2* gridSize, Vector2* gridStart) {
    float desiredCellWidth = (boundary.width - 1.0f) / g_gridCols;
    float desiredCellHeight = (boundary.height - 1.0f) / g_gridRows;
    *cellSize = floorf(fminf(desiredCellWidth, desiredCellHeight));
    gridSize->x = *cellSize * g_gridCols;
    gridSize->y = *cellSize * g_gridRows;
    gridStart->x = (int)(boundary.x + (boundary.width - gridSize->x) / 2.0f) + 0.5f;
    gridStart->y = (int)(boundary.y + (boundary.height - gridSize->y) / 2.0f) + 0.5f;
}

static Int2 GetCellID(Int2 mousePos, Rectangle gridBoundary) {
    Int2 cell = { -1, -1 };

    float cellSize = 0;
    Vector2 gridSize, gridStart;
    GetGridData(gridBoundary, &cellSize, &gridSize, &gridStart);

    if ((mousePos.x < gridStart.x) || (mousePos.y < gridStart.y))
        return cell;

    if ((mousePos.x > (gridStart.x + gridSize.x)) || (mousePos.y > (gridStart.y + gridSize.y)))
        return cell;

    cell.x = (int)((mousePos.x - gridStart.x) / cellSize);
    cell.y = (int)((mousePos.y - gridStart.y) / cellSize);

    return cell;
}

static float GetArrowRotation(Int2* path, size_t pathLen, int i, Int2 goal) {
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

    rotation = atan2f((float)dir.y, (float)dir.x);
    rotation = rotation * 180.0f / PI;

    return rotation;
}

static void GridResize() {
    // Store blocked cells
    bool* blockCells = malloc(g_gridRowsOld * g_gridColsOld * sizeof(bool));
    for (int row = 0; row < g_gridRowsOld; row++) {
        for (int col = 0; col < g_gridColsOld; col++) {
            bool blocked = AStarIsBlocked(col, row);
            blockCells[row * g_gridColsOld + col] = blocked;
        }
    }

    AStarDestroy();

    // Config a new A Star and set blocked cells
    AStarInit(g_gridCols, g_gridRows, g_gridNavMode);
    for (int row = 0; row < g_gridRows; row++) {
        for (int col = 0; col < g_gridCols; col++) {
            if ((col < g_gridColsOld) && (row < g_gridRowsOld)) {
                bool blocked = blockCells[row * g_gridColsOld + col];
                AStarSetBlocked(col, row, blocked);
            }
        }
    }

    free(blockCells);

    // Check if start remains valid
    if ((g_start.x >= g_gridCols) || (g_start.y >= g_gridRows)) {
        g_start.x = -1;
        g_start.y = -1;
    }

    // Check if goal remains valid
    if ((g_goal.x >= g_gridCols) || (g_goal.y >= g_gridRows)) {
        g_goal.x = -1;
        g_goal.y = -1;
    }

    if (Int2IsValid(g_start) && Int2IsValid(g_goal))
        AStarSearch(g_start, g_goal);
}

static void MouseLeftPressed(Int2 cell) {
    bool modified = false;

    // Set start
    if (!Int2IsValid(g_start)) {
        g_start = cell;
        AStarSetBlocked(cell.x, cell.y, false);
        modified = true;
    }

    //Set goal
    if (!modified && !Int2IsValid(g_goal) && (!Int2Equals(cell, g_start))) {
        g_goal = cell;
        AStarSetBlocked(cell.x, cell.y, false);
        modified = true;
    }

    // Reset start
    if (!modified && Int2Equals(g_start, cell)) {
        g_start = (Int2){ -1, -1 };
        modified = true;
    }

    // Reset goal
    if (!modified && Int2Equals(g_goal, cell)) {
        g_goal = (Int2){ -1, -1 };
        modified = true;
    }

    // Set / Reset block
    if (!modified) {
        bool blocked = AStarIsBlocked(cell.x, cell.y);
        g_cellAction = blocked ? UNBLOCK : BLOCK;
        AStarSetBlocked(cell.x, cell.y, !blocked);
        modified = true;
    }
}

static void Update() {
    g_gridBoundary = (Rectangle){ 20.0f, 50.0f, GetScreenWidth() - 40.0f, GetScreenHeight() - 70.0f };

    // If user changed rows or cols, resize grid
    if ((g_gridColsOld != g_gridCols) || (g_gridRowsOld != g_gridRows) || (g_gridNavModeOld != g_gridNavMode)) {
        GridResize();
        g_gridColsOld = g_gridCols;
        g_gridRowsOld = g_gridRows;
        g_gridNavModeOld = g_gridNavMode;
    }

    // Disable input if modifying gui control
    bool gridEnabled = !g_editModeNavMode;
    if (!gridEnabled)
        return;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Int2 cell = GetCellID((Int2){ GetMouseX(), GetMouseY() }, g_gridBoundary);

        if (Int2IsValid(cell)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { // Pressed on this frame
                MouseLeftPressed(cell);
            }
            else {                                         // Pressed on previous frames
                if (g_cellAction != NONE) {
                    // Quick blocking/unblocking without releasing the mouse button
                    bool blocked = g_cellAction == BLOCK ? true : false;
                    AStarSetBlocked(cell.x, cell.y, blocked);
                }
            }
        }
    }

    if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        g_cellAction = NONE;
    }

    // On mouse released, if start and goal valid, run A* algorithm
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        Int2 cell = GetCellID((Int2) { GetMouseX(), GetMouseY() }, g_gridBoundary);
        if (Int2IsValid(g_start) && Int2IsValid(g_goal) && Int2IsValid(cell))
            AStarSearch(g_start, g_goal);
    }
}

static void DrawGUI(Rectangle boundary) {
    // Background
    DrawRectangleRec(boundary, ColorAlpha(WHITE, 0.15f));

    float offsetX = boundary.x + 30.0f;

    DrawText("Grid size:", (int)offsetX, (int)(boundary.y + 10.0f), 20, WHITE);
    offsetX += 100.0f;

    static bool editModeWidth = false;
    static bool editModeHeight = false;

    // Grid columns
    if (GuiSpinner((Rectangle) { offsetX, boundary.y + 10.0f, 100.0f, 20.0f }, "", &g_gridCols, 5, 160, editModeWidth))
        editModeWidth = !editModeWidth;
    offsetX += 100.0f + 5.0f;

    // Grid rows
    if (GuiSpinner((Rectangle) { offsetX, boundary.y + 10.0f, 100.0f, 20.0f }, "", &g_gridRows, 5, 100, editModeHeight))
        editModeHeight = !editModeHeight;
    offsetX += 100.0f + 20.0f;

    DrawText("Direction:", (int)offsetX, 10, 20, WHITE);
    offsetX += 100;

    // Navigation mode
    if (GuiDropdownBox((Rectangle) { offsetX, boundary.y + 10.0f, 100.0f, 20.0f }, "Four;Eight", (int *)&g_gridNavMode, g_editModeNavMode))
        g_editModeNavMode = !g_editModeNavMode;

    const char* t = TextFormat("FPS: %d", GetFPS());
    DrawText(t, (int)(boundary.width - 100.0f - 10.0f), (int)(boundary.y + 10.0f), 20, WHITE);
}

static void DrawPath(Vector2 gridStart, float cellSize) {
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
            float alpha = 0.2f;
            float dif = (float)fabs(time - (double)i);
            if (dif < 1.5f) {
                dif = dif / 1.5f;
                alpha = sinf((1.0f - dif) * PI/2.0f) * (1.0f - alpha) + alpha;
            }

            DrawTexturePro(
                g_arrow,
                (Rectangle) { 0, 0, (float)g_arrow.width, (float)g_arrow.height },
                (Rectangle) {
                    gridStart.x + (cellSize * p->x) + cellSize * 0.5f,
                    gridStart.y + (cellSize * p->y) + cellSize * 0.5f,
                    cellSize * 0.5f,
                    cellSize * 0.5f
                },
                (Vector2)   { cellSize * 0.25f, cellSize * 0.25f },
                rotation,
                ColorAlpha(WHITE, alpha));
        }
    }
}

static void DrawGridRectangle(Int2 pos, Vector2 gridStart, float cellSize, bool scaleAnim, float hue, float saturation) {
    rlPushMatrix();
    
    float scale = 1.0f;
    if (scaleAnim)
        scale = 1.0f + (float)sin(GetTime()) * 0.1f;

    rlTranslatef(gridStart.x + (cellSize * pos.x) + cellSize * 0.5f + 2.0f, gridStart.y + (cellSize * pos.y) + cellSize * 0.5f + 2.0f, 0);
    rlScalef(scale, scale, 1.0f);

    // Shadow
    Rectangle rectBG = { -cellSize * 0.5f+4, -cellSize * 0.5f+4, (float)(cellSize - 9), (float)(cellSize - 9) };
    DrawRectangleRounded(rectBG, 0.5f, 4, GRAY);

    // Exterior color
    Rectangle rectFG1 = { -cellSize*0.5f, -cellSize*0.5f, cellSize - 7.0f, cellSize - 7.0f };
    DrawRectangleRounded(rectFG1, 0.5f, 4, ColorFromHSV(hue, saturation, 1.0f));

    // Interior color
    Rectangle rectFG2 = { -cellSize*0.5f+4.0f, -cellSize*0.5f+4.0f, cellSize - 15.0f, cellSize - 15.0f };
    DrawRectangleRounded(rectFG2, 0.5f, 4, ColorFromHSV(hue, saturation, 0.8f));

    rlPopMatrix();
}

static float Lerp(float from, float to, float rel) {
    return ((1.0f - rel) * from) + (rel * to);
}

static float InvLerp(float from, float to, float value) {
    return (value - from) / (to - from);
}

static float Clamp(float min, float max, float value) {
    return fminf(max, fmaxf(min, value));
}

static float InvLerpClamp01(float from, float to, float value) {
    return Clamp(0, 1, InvLerp(from, to, value));
}

static void DrawGridAStar(Rectangle boundary) {
    float cellSize = 0;
    Vector2 gridSize, gridStart;
    GetGridData(boundary, &cellSize, &gridSize, &gridStart);

    float alpha = InvLerpClamp01(8, 20, cellSize);
    alpha = alpha * 0.7f + 0.3f;

    // Vertical lines
    float xOffset = gridStart.x;
    for (int col = 0; col < g_gridCols + 1; col++) {
        DrawLineV((Vector2) { xOffset, gridStart.y }, (Vector2) { xOffset, gridStart.y + gridSize.y }, ColorAlpha(WHITE, alpha));
        xOffset += cellSize;
    }

    // Horizontal lines
    float yOffset = gridStart.y;
    for (int row = 0; row < g_gridRows + 1; row++) {
        DrawLineV((Vector2) { gridStart.x, yOffset }, (Vector2) { gridStart.x + gridSize.x, yOffset }, ColorAlpha(WHITE, alpha));
        yOffset += cellSize;
    }

    // Start and goal cells
    bool startValid = Int2IsValid(g_start);
    bool goalValid  = Int2IsValid(g_goal);
    if (startValid)
        DrawGridRectangle(g_start, gridStart, cellSize, true, 0.0f, 1.0f);

    if (goalValid)
        DrawGridRectangle(g_goal, gridStart, cellSize, true, 120.0f, 1.0f);

    // Blocking and visited cells
    for (int row = 0; row < g_gridRows; row++) {
        for (int col = 0; col < g_gridCols; col++) {
            if (AStarIsBlocked(col, row))
                DrawGridRectangle((Int2) { col, row }, gridStart, cellSize, false, 0.0f, 0.0f);
            else if (AStarIsVisited(col, row)) {
                if (startValid && goalValid)
                    DrawRectangleRec(
                        (Rectangle) {
                            gridStart.x + 1 + col * cellSize,
                            gridStart.y + 1 + row * cellSize,
                            cellSize - 1,
                            cellSize - 1
                        },
                        (Color) { 40, 240, 40, 30 }
                    );
            }
        }
    }

    DrawPath(gridStart, cellSize);
}

static void Draw() {
    DrawGridAStar(g_gridBoundary);

    DrawGUI((Rectangle){ 0, 0, (float)GetScreenWidth(), 35.0f });
}

static void SaveGrid() {
    printf("Saving grid...\n");

    FILE* fp = fopen("grid.bin", "wb");

    fwrite(&g_gridCols, 1, sizeof(int), fp);
    fwrite(&g_gridRows, 1, sizeof(int), fp);
    fwrite(&g_gridNavMode, 1, sizeof(int), fp);
    fwrite(&g_start, 1, sizeof(Int2), fp);
    fwrite(&g_goal, 1, sizeof(Int2), fp);
    for (int row = 0; row < g_gridRows; row++) {
        for (int col = 0; col < g_gridCols; col++) {
            bool blocked = AStarIsBlocked(col, row);
            fwrite(&blocked, 1, sizeof(bool), fp);
        }
    }

    fclose(fp);

    printf("Grid saved\n");
}

static void LoadGrid() {
    printf("Loading grid...\n");

    FILE* fp = fopen("grid.bin", "rb");

    if (fp == NULL) {
        printf("[Error]: File not found\n");
        return;
    }

    int cols, rows, navMode;
    fread(&cols, 1, sizeof(int), fp);
    fread(&rows, 1, sizeof(int), fp);
    fread(&navMode, 1, sizeof(int), fp);

    if ((cols != g_gridCols) || (rows != g_gridRows) || (navMode != g_gridNavMode))
        AStarInit(cols, rows, navMode);

    g_gridCols = cols;
    g_gridRows = rows;
    g_gridNavMode = navMode;

    fread(&g_start, 1, sizeof(Int2), fp);
    fread(&g_goal, 1, sizeof(Int2), fp);
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            bool blocked = false;
            fread(&blocked, 1, sizeof(bool), fp);
            AStarSetBlocked(col, row, blocked);
        }
    }

    if (Int2IsValid(g_start) && Int2IsValid(g_goal))
        AStarSearch(g_start, g_goal);

    printf("Grid loaded\n");

    fclose(fp);
}

void main() {
    g_gridColsOld = g_gridCols;
    g_gridRowsOld = g_gridRows;
    AStarInit(g_gridCols, g_gridRows, g_gridNavMode);

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "A*");

    g_arrow = LoadTexture("resources/white-arrow.png");

    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    LoadGrid();

    while (!WindowShouldClose()) {
        Update();

        BeginDrawing();

        ClearBackground((Color) {28, 28, 28, 255});
        Draw();

        EndDrawing();
    }

    SaveGrid();

    CloseWindow();
    AStarDestroy();
}