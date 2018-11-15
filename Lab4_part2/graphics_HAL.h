/*
 * graphics_HAL.h
 *
 *  Created on: Oct 18, 2018
 *      Author: Leyla
 */

#ifndef GRAPHICS_HAL_H_
#define GRAPHICS_HAL_H_

#include <ti/grlib/grlib.h>

void draw_Base(Graphics_Context *g_sContext_p);
void MoveCircle(Graphics_Context *g_sContext_p, bool moveToLeft, bool moveToRight);
void InitGraphics(Graphics_Context *g_sContext_p);
void drawXY(Graphics_Context *g_sContext_p, unsigned int x, unsigned int y);
void printList_blocking(Graphics_Context *g_sContext_p, int n);
bool printList_nonBlocking(Graphics_Context *g_sContext_p, int n);
void drawAccelData(Graphics_Context *g_sContext_p, uint16_t *resultsBuffer);

#endif /* GRAPHICS_HAL_H_ */
