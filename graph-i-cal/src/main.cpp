/*
 *  main.cpp
 *
 */

#include <stdio.h>
#include <memory.h>
#include <cmath>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS

#include "raylib.h"
#include "raygui.h"

typedef struct _grid2d 
{
    Vector2 originScreenSpace;
    Vector2 screenDimensions;
    Vector2 horizontalAxis[2];
    Vector2 verticalAxis[2];
    float zoom;
    union _x_range
    {
        struct 
        {
            int     r[2];
            int     high;
            int     low;
        };
    } xRange;
    union _y_range
    {
        struct 
        {
            int     r[2];
            int     high;
            int     low;
        };
    } yRange;
} Grid2D;

void
Draw_2D_Grid(Grid2D g);

void
Draw_Cursor(Grid2D g); 


float
Vector2_Magnitude(Vector2 v);

Vector2
Vector2_Normalize(Vector2 v);

float
Vector2_DotProduct(Vector2 v1, Vector2 v2);

float
Vector2_Angle_Between_Vectors(Vector2 a, Vector2 b);

float 
Radians_To_Degrees(float radians);

Vector2
Normalize_To_Screen_Space(Grid2D g, Vector2 normal);

Vector2
Normalize_To_Grid_Space(Grid2D g, Vector2 v);

Vector2
Vector2_Add(Vector2 v1, Vector2 v2);

Vector2
Screen_Space_To_Grid_Space(Grid2D g, Vector2 v);

int 
main(void)
{
    Grid2D grid = { 0 };
    // screen dimensions
    grid.screenDimensions.x = 1280;
    grid.screenDimensions.y = 720;
    // screen space origin coordinates
    grid.originScreenSpace.x = grid.screenDimensions.x / 2;
    grid.originScreenSpace.y = grid.screenDimensions.y / 2;
    // positive x-axis
    grid.horizontalAxis[0].x = 1;  
    grid.horizontalAxis[0].y = 0;
    // negative x-axis
    grid.horizontalAxis[1].x = -1;
    grid.horizontalAxis[1].y = 0;
    // positive y-axis
    grid.verticalAxis[0].x = 0;  
    grid.verticalAxis[0].y = 1;
    // negative y-axis
    grid.verticalAxis[1].x = 0;  
    grid.verticalAxis[1].y = -1;
    // x grid range [-20, 20]
    grid.xRange.low = -20;
    grid.xRange.high = 20;
    // y grid range [-20, 20]
    grid.yRange.low = -20;
    grid.yRange.high = 20;
    
    grid.zoom = 1.0f;
    
    InitWindow(grid.screenDimensions.x, grid.screenDimensions.y, "default grid");
    SetTargetFPS(60);            
    
    int dropdownBoxActive = 0;
    bool dropdownBoxEditMode = false;
    Rectangle box = { 0 };
    box.height = 25;
    box.width = 65;
    box.x = 125;
    box.y = 30;
    
    while (!WindowShouldClose()) 
    {
        grid.zoom += ((float)GetMouseWheelMove() * 0.05f);
        if (grid.zoom > 3.0f)
        {
            grid.zoom = 3.0f;
        }
        else if (grid.zoom < 0.1f)
        {
            grid.zoom = 0.1f;
        }
        // draw
        BeginDrawing();
        {
            GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            ClearBackground(BLACK); 
            Draw_2D_Grid(grid);
            Draw_Cursor(grid);
            /*
            if (GuiDropdownBox(box, "#01#ONE;#02#TWO;#03#THREE;#04#FOUR", &dropdownBoxActive, &dropdownBoxEditMode)) 
            {
                dropdownBoxEditMode = !dropdownBoxEditMode;
            }
*/
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void 
Draw_2D_Grid(Grid2D g)
{
    Color color;
    color.r = 130;
    color.b = 130;
    color.g = 130;
    color.a = 128;
    
    char buff[256] = { 0 };
    
    // top left of grid space
    Vector2 v1;
    v1.x = g.xRange.low;
    v1.y = g.yRange.high;
    
    // top right of grid space
    Vector2 v2;
    v2.x = g.xRange.high;
    v2.y = g.yRange.high;
    
    Vector2 v1Normalized  = { 0 };
    Vector2 v2Normalized  = { 0 };
    Vector2 v1ScreenSpace = { 0 };
    Vector2 v2ScreenSpace = { 0 };
    
    // draw horizontal grid lines
    for (int i = g.yRange.low; i <= g.yRange.high; i++)
    {
        v1Normalized = Normalize_To_Grid_Space(g, v1);
        v2Normalized = Normalize_To_Grid_Space(g, v2);
        v1ScreenSpace = Normalize_To_Screen_Space(g, v1Normalized);
        v2ScreenSpace = Normalize_To_Screen_Space(g, v2Normalized);
        DrawLineEx(v1ScreenSpace, v2ScreenSpace, 2.0f, color);
        v1.y--;
        v2.y--;
    }
    
    // top left of grid space
    v1.x = g.xRange.low;
    v1.y = g.yRange.high;
    
    // bottom left of grid space
    v2.x = g.xRange.low;
    v2.y = g.yRange.low;
    
    // draw horizontal grid lines
    for (int i = g.xRange.low; i <= g.xRange.high; i++)
    {
        v1Normalized = Normalize_To_Grid_Space(g, v1);
        v2Normalized = Normalize_To_Grid_Space(g, v2);
        v1ScreenSpace = Normalize_To_Screen_Space(g, v1Normalized);
        v2ScreenSpace = Normalize_To_Screen_Space(g, v2Normalized);
        DrawLineEx(v1ScreenSpace, v2ScreenSpace, 1.0f, color);
        v1.x++;
        v2.x++;
    }
    
    // negative x-axis
    v1Normalized.x = g.horizontalAxis[1].x;
    v1Normalized.y = g.horizontalAxis[1].y;
    
    // positive x-axis
    v2Normalized.x = g.horizontalAxis[0].x;
    v2Normalized.y = g.horizontalAxis[0].y;
    
    // draw x-axis
    v1ScreenSpace = Normalize_To_Screen_Space(g, v1Normalized);
    v2ScreenSpace = Normalize_To_Screen_Space(g, v2Normalized);
    DrawLineEx(v1ScreenSpace, v2ScreenSpace, 2.0f, RAYWHITE);
    
    // position y-axis
    v1Normalized.x = g.verticalAxis[0].x;
    v1Normalized.y = g.verticalAxis[0].y;
    
    // negative y-axis
    v2Normalized.x = g.verticalAxis[1].x;
    v2Normalized.y = g.verticalAxis[1].y;
    
    // draw y-axis
    v1ScreenSpace = Normalize_To_Screen_Space(g, v1Normalized);
    v2ScreenSpace = Normalize_To_Screen_Space(g, v2Normalized);
    DrawLineEx(v1ScreenSpace, v2ScreenSpace, 2.0f, RAYWHITE);
}

void
Draw_Cursor(Grid2D g)
{
    Vector2 mScreenSpace = GetMousePosition();
    DrawLineEx(g.originScreenSpace, mScreenSpace, 2.0f, RED);
    Vector2 mGridSpace = Screen_Space_To_Grid_Space(g, mScreenSpace);
    char buff[256] = { 0 };
    memset(buff, 0, 256);
    sprintf(buff, "grid coordinates: (%f, %f)", mGridSpace.x, mGridSpace.y);
    DrawText(buff, g.originScreenSpace.x + 20, g.originScreenSpace.y + 20, 8, RAYWHITE);
}

float
Vector2_Magnitude(Vector2 v)
{
    return sqrtf(Vector2_DotProduct(v, v));
}

Vector2
Vector2_Normalize(Vector2 v)
{
    float magnitude = Vector2_Magnitude(v);
    Vector2 r;
    r.x = v.x / magnitude;
    r.y = v.y / magnitude;
    return r;
}

float
Vector2_DotProduct(Vector2 v1, Vector2 v2) 
{ 
    return (v1.x * v2.x) + (v1.y * v2.y);
}

float
Vector2_Angle_Between_Vectors(Vector2 a, Vector2 b)
{
    float aMag = Vector2_Magnitude(a);
    float bMag = Vector2_Magnitude(b);
    if (aMag == 0 || bMag == 0)
    {
        return 0.0f;
    }
    float v = Vector2_DotProduct(a, b);
    v = v / (aMag * bMag);
    _ASSERT(v <= 1.0f && v >= -1.0f);
    v = acosf(v);
    return v;
}

float 
Radians_To_Degrees(float radians)
{
    return (radians * (180 / PI));
}

Vector2
Vector2_Add(Vector2 v1, Vector2 v2)
{
    Vector2 r = { 0 };
    r.x = v1.x + v2.x;
    r.y = v1.y + v2.y;
    return r;
}

// Vector2 components needs to be within the range
// of Grid2D x and y ranges since these are basically
// the magnitude of the grid spaces vectors
Vector2
Normalize_To_Grid_Space(Grid2D g, Vector2 v)
{
    Vector2 r = { 0 };
    r.x = v.x / g.xRange.high;
    r.y = v.y / g.yRange.high;
    return r;
}

Vector2
Normalize_To_Screen_Space(Grid2D g, Vector2 normal)
{
    Vector2 r = { 0 };
    // screen space actual origin (0, 0) is top left of screen,
    // with x-axis going positive in toward the right hand side
    // and the y-axis increasing going toward the bottom of the
    // screen: 
    // 0--------> +x
    // |
    // |
    // |
    // v +y
    
    // original implementation
    //r.x = g.originScreenSpace.x + (normal.x * g.originScreenSpace.x); 
    //r.y = g.originScreenSpace.y + (-normal.y * g.originScreenSpace.y);
    // secondary implementation
    //r.x = g.originScreenSpace.x + (normal.x * g.originScreenSpace.y);
    //r.y = g.originScreenSpace.y + (-normal.y * g.originScreenSpace.y);
    
    r.x = g.originScreenSpace.x + ((normal.x * g.originScreenSpace.x) * g.zoom);
    r.y = g.originScreenSpace.y + ((-normal.y * g.originScreenSpace.x) * g.zoom);
    return r;
}

Vector2
Screen_Space_To_Grid_Space(Grid2D g, Vector2 v)
{
    Vector2 r = { 0 };
    r.x =  (v.x - g.originScreenSpace.x) / g.zoom;
    r.y = -(v.y - g.originScreenSpace.y) / g.zoom;
    r.x /= g.originScreenSpace.x;
    r.y /= g.originScreenSpace.x;
    r.x *= g.xRange.high;
    r.y *= g.yRange.high;
    return r;
}