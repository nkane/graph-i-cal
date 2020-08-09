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
    Vector2 currrentLocalCoordinates;
    float   currentAngleRadians;
    float   currentAngleDegrees;
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

// TODO(nick): 
// - might be able to pack options in to a int64
typedef struct _vectorControlState
{
    bool displayAngle;
    bool displayCosine;
    bool displaySine;
} VectorControlState;

void
Draw_2D_Grid(Grid2D g);

void
Draw_Cursor(Grid2D *g, VectorControlState *s); 

void
Draw_GUI(Grid2D *g, VectorControlState *s);

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

Vector2
Vector2_Rotate(Vector2 v, float radians);

int 
main(void)
{
    Grid2D grid = {};
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
    // grid current zoom level
    grid.zoom = 1.0f;
    // current local coordinates
    grid.currrentLocalCoordinates.x = 0;
    grid.currrentLocalCoordinates.y = 0;

    VectorControlState vectorControlState = {};
    
    InitWindow(grid.screenDimensions.x, grid.screenDimensions.y, "default grid");
    HideCursor();
    SetTargetFPS(60);            
    
    while (!WindowShouldClose()) 
    {
        // TODO(nick): place in another function
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
            ClearBackground(BLACK); 
            Draw_2D_Grid(grid);
            Draw_Cursor(&grid,&vectorControlState);
            Draw_GUI(&grid, &vectorControlState);
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
    
    char buff[256] = {};
    
    // top left of grid space
    Vector2 v1;
    v1.x = g.xRange.low;
    v1.y = g.yRange.high;
    
    // top right of grid space
    Vector2 v2;
    v2.x = g.xRange.high;
    v2.y = g.yRange.high;
    
    Vector2 v1Normalized  = {};
    Vector2 v2Normalized  = {};
    Vector2 v1ScreenSpace = {};
    Vector2 v2ScreenSpace = {};
    
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
Draw_Cursor(Grid2D *g, VectorControlState *s)
{
    Vector2 mScreenSpace = GetMousePosition();
#if 1 
    // NOTE(nick): just for testing (1, 1) -> 45 deg
    mScreenSpace.x = 1.0f;
    mScreenSpace.y = 1.0f;
    mScreenSpace = Normalize_To_Grid_Space(*g, mScreenSpace);
    mScreenSpace = Normalize_To_Screen_Space(*g, mScreenSpace);
#endif
    // draw cursor vector
    DrawLineEx(g->originScreenSpace, mScreenSpace, 2.0f, RED);
    // TODO(nick): might be able to pull this out?
    g->currrentLocalCoordinates = Screen_Space_To_Grid_Space(*g, mScreenSpace);
    Vector2 normalizedCoordinates = Vector2_Normalize(g->currrentLocalCoordinates);
    g->currentAngleRadians = Vector2_Angle_Between_Vectors(g->horizontalAxis[0], normalizedCoordinates);
    g->currentAngleDegrees = Radians_To_Degrees(g->currentAngleRadians);
    // we are in third and fourth quadrant, need to add 180 degrees
    if (normalizedCoordinates.y < 0)
    {
        g->currentAngleDegrees = 180 + (180 - g->currentAngleDegrees);
    }
    // NOTE(nick): just draw a simple triangle at the origin of the grid

#if 0
    Vector2 t1 = {};
    t1.x = 0.0f;
    t1.y = 1.0f;
    Vector2 t2 = {};
    t2.x = t1.x - 1.0f;
    t2.y = t1.y - 1.0f;
    Vector2 t3 = {};
    t3.x = t1.x + 1.0f;
    t3.y = t1.y - 1.0f;
#else
    Vector2 t1 = Screen_Space_To_Grid_Space(*g, mScreenSpace);
    Vector2 t2 = {};
    t2.x = t1.x - 0.25f;
    t2.y = t1.y - 0.25f;
    Vector2 t3 = {};
    t3.x = t1.x + 0.25f;
    t3.y = t2.y;
#endif

    t1 = Normalize_To_Grid_Space(*g, t1);
    t2 = Normalize_To_Grid_Space(*g, t2);
    t3 = Normalize_To_Grid_Space(*g, t3);

    t1 = Normalize_To_Screen_Space(*g, t1);
    t2 = Normalize_To_Screen_Space(*g, t2);
    t3 = Normalize_To_Screen_Space(*g, t3);
    // vertex order is counterclockwise
    DrawTriangle(t1, t2, t3, GREEN);
    // draw cosine
    if (s->displayCosine)
    {
        Vector2 cursorCosine = {};
        cursorCosine.x = mScreenSpace.x;
        cursorCosine.y = g->originScreenSpace.y;
        DrawLineEx(g->originScreenSpace, cursorCosine, 2.0f, BLUE);
        DrawLineEx(mScreenSpace, cursorCosine, 2.0f, BLUE);
    }
    // draw sine
    if (s->displaySine)
    {
        Vector2 cursorSine = {};
        cursorSine.x = g->originScreenSpace.x;
        cursorSine.y = mScreenSpace.y;
        DrawLineEx(g->originScreenSpace, cursorSine, 2.0f, GREEN);
        DrawLineEx(mScreenSpace, cursorSine, 2.0f, GREEN);
    }
    // draw circle that represents angle
    if (s->displayAngle)
    {
        DrawCircleSectorLines(g->originScreenSpace, 20.0f, 90, 90 + (int)g->currentAngleDegrees, 1000, YELLOW);
    }
}

// TODO(nick): 
// - normalize all of these coordinates
// - rename variables
// - use control state structure
void
Draw_GUI(Grid2D *g, VectorControlState *s)
{
    // load gui style
    GuiLoadStyle("styles/ashes/ashes.rgs");
    
    Rectangle controlBoxRectangle;
    controlBoxRectangle.width = g->screenDimensions.x / 8;
    controlBoxRectangle.height = g->screenDimensions.y / 4;
    controlBoxRectangle.x = 10;
    controlBoxRectangle.y = 10; 
    
    Rectangle labelRectangle;
    labelRectangle.x = controlBoxRectangle.x + 5;
    labelRectangle.y = controlBoxRectangle.y + 20;
    labelRectangle.width = 110;
    labelRectangle.height = 10; 
    
    int origY = labelRectangle.y;
    int tempY = labelRectangle.y + 20;
    
    char buffer[1024] = {};
    memset(buffer, 0, 1024);
    sprintf(buffer, "cursor: (%.2f, %.2f)", g->currrentLocalCoordinates.x, g->currrentLocalCoordinates.y);
    memset(buffer, 0, 1024);
    sprintf(buffer, "angle: %.2f", g->currentAngleDegrees);
    
    Color background;
    background.r = 125;
    background.g = 125;
    background.b = 125;
    background.a = 175;
    
    // draw coordinate information
    DrawRectangleRec(controlBoxRectangle, background);
    GuiGroupBox(controlBoxRectangle, "Coordinate Information");
    memset(buffer, 0, 1024);
    labelRectangle.y = origY;
    sprintf(buffer, "cursor: (%.2f, %.2f)", g->currrentLocalCoordinates.x, g->currrentLocalCoordinates.y);
    GuiLabel(labelRectangle, buffer);
    memset(buffer, 0, 1024);
    sprintf(buffer, "angle: %.2f", g->currentAngleDegrees);
    labelRectangle.y = tempY;
    GuiLabel(labelRectangle, buffer);

    // draw check boxes
    tempY += 20;
    labelRectangle.y = tempY;
    labelRectangle.width = 10;
    labelRectangle.height = 10;
    s->displayAngle = GuiCheckBox(labelRectangle, "angle", s->displayAngle);
    tempY += 20;
    labelRectangle.y = tempY;
    s->displayCosine = GuiCheckBox(labelRectangle, "cosine", s->displayCosine);
    tempY += 20;
    labelRectangle.y = tempY;
    s->displaySine = GuiCheckBox(labelRectangle, "sine", s->displaySine);
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
    Vector2 r = {};
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
    Vector2 r = {};
    r.x = v.x / g.xRange.high;
    r.y = v.y / g.yRange.high;
    return r;
}

Vector2
Normalize_To_Screen_Space(Grid2D g, Vector2 normal)
{
    Vector2 r = {};
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
    Vector2 r = {};
    r.x =  (v.x - g.originScreenSpace.x) / g.zoom;
    r.y = -(v.y - g.originScreenSpace.y) / g.zoom;
    r.x /= g.originScreenSpace.x;
    r.y /= g.originScreenSpace.x;
    r.x *= g.xRange.high;
    r.y *= g.yRange.high;
    return r;
}

Vector2
Vector2_Rotate(Vector2 v, float radians)
{
    Vector2 r = {};
    float rotationMatrix[4] = 
    {
        cos(radians), -sin(radians),
        sin(radians),  cos(radians),
    };
    r.x = Vector2_DotProduct(v, *((Vector2 *)rotationMatrix));
    r.y = Vector2_DotProduct(v, *((Vector2 *)(rotationMatrix + 2)));
    return r;
}