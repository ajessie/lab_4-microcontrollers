/*
 * graphics_HAL.h
 *
 *  Created on: Oct 18, 2018
 *      Author: Leyla
 */

#ifndef GRAPHICS_HAL_H_
#define GRAPHICS_HAL_H_
#define VX_ZERO_GEAR    0x19BD

#include <ti/grlib/grlib.h>
#include "labyrinth.h"

typedef enum {zero,one,two} gears;


typedef struct Speeds
{
    gears Speed;
    int Vx;
    int Vy;
    int count;
} Speeds;


void draw_Base(Graphics_Context *g_sContext_p);
void MoveCircle(Graphics_Context *g_sContext_p, bool moveToLeft, bool moveToRight);
void InitGraphics(Graphics_Context *g_sContext_p);
void drawXY(Graphics_Context *g_sContext_p, unsigned int x, unsigned int y);
void printList_blocking(Graphics_Context *g_sContext_p, int n);
bool printList_nonBlocking(Graphics_Context *g_sContext_p, int n);
void drawAccelData(Graphics_Context *g_sContext_p, uint16_t *resultsBuffer);
void DrawBall(Graphics_Context *g_sContext_p, marble_t *marble);
void DrawVxVy(Graphics_Context *g_sContext_p);
void WriteSpeed(Speeds *speed, Graphics_Context *g_sContext_p);
void GearShift(uint16_t *resultsBuffer, Speeds *speed, Graphics_Context *g_sContext_p);
void DrawTop(Graphics_Context *g_sContext_p);
void DrawBottom(Graphics_Context *g_sContext_p);
void DrawRight(Graphics_Context *g_sContext_p);
void DrawLeft(Graphics_Context *g_sContext_p);
void DrawWalls(Graphics_Context *g_sContext_p);
void DrawEasyStage(Graphics_Context *g_sContext_p);
void DrawEasyHole(Graphics_Context *g_sContext_p);
void DrawHoles(Graphics_Context *g_sContext_p);
void MoveBall(Graphics_Context *g_sContext_p, marble_t *marble, Speeds *speed);

#endif /* GRAPHICS_HAL_H_ */
