/*
 * graphics_HAL.c
 *
 *  Created on: Oct 18, 2018
 *      Author: Leyla
 */

#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "Timer32_HAL.h"
#include "graphics_HAL.h"

// 100ms in us unit is 100,000
#define T10MS_IN_US 10000
#define BLOCKING_WAIT_TIME 1000000


#define BALL_Y_STEP 10                   // The ball moves in y direction 10 pixesl per step
#define BALL_TIME_STEP T10MS_IN_US      // We update the location of the ball evey 100 ms
// The above two numbers result in 10/100ms = 10/0.1s = 100 pixel/sec movement for the ball

extern HWTimer_t timer0, timer1;


void printList_blocking(Graphics_Context *g_sContext_p, int n)
{

    int8_t string[2];

    string[1] = 0;


    int x = 10;
    int i;

    // The n is the maximum we should print, we just make sure n is not more than 9 (single digit)
     if (n>9)
         n = 9;

    OneShotSWTimer_t waitTimer;
    InitOneShotSWTimer(&waitTimer, &timer0, BLOCKING_WAIT_TIME);

    for (i = 0; i<n; i++) {
        string[0] = i+'0';

        Graphics_drawString(g_sContext_p,string , -1, x, 5, true);

        x = x + 10;

        StartOneShotSWTimer(&waitTimer);

        while (!OneShotSWTimerExpired(&waitTimer)) ;

    }

}

void DrawBall(Graphics_Context *g_sContext_p){                                                              //Draw the green circle at bottom of the screen

    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_GREEN);
    Graphics_fillCircle(g_sContext_p, 64, 106, 2);
}

void DrawTop(Graphics_Context *g_sContext_p){
    uint16_t x_pos = 40, y_pos = 60;
    int i;
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_RED);
    for (i =0; i < 60; i++){
        x_pos++;
        Graphics_drawPixel(g_sContext_p, x_pos, y_pos);
    }
}

void DrawBottom(Graphics_Context *g_sContext_p){
    uint16_t x_pos = 40, y_pos = 110;
    int i;
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_RED);
    for (i =0; i < 60 ; i++){
        x_pos++;
        Graphics_drawPixel(g_sContext_p, x_pos, y_pos);
    }
}

void DrawRight(Graphics_Context *g_sContext_p){
    uint16_t x_pos = 100, y_pos = 60;
    int i;
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_RED);
    for (i =0; i < 50; i++){
        y_pos++;
        Graphics_drawPixel(g_sContext_p, x_pos, y_pos);
    }
}

void DrawLeft(Graphics_Context *g_sContext_p){
    uint16_t x_pos = 40, y_pos = 60;
    int i;
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_RED);
    for (i =0; i < 50; i++){
        y_pos++;
        Graphics_drawPixel(g_sContext_p, x_pos, y_pos);
    }
}

void DrawWalls(Graphics_Context *g_sContext_p){
    DrawTop(g_sContext_p);
    DrawBottom(g_sContext_p);
    DrawRight(g_sContext_p);
    DrawLeft(g_sContext_p);
}

void DrawEasyStage(Graphics_Context *g_sContext_p){
    uint16_t x_pos = 74, y_pos = 110;
    int i;
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_RED);
    for (i =0; i < 30; i++){
        y_pos--;
        Graphics_drawPixel(g_sContext_p, x_pos, y_pos);
    }

    DrawEasyHole(g_sContext_p);
}

void DrawEasyHole(Graphics_Context *g_sContext_p){
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_BLACK);
    Graphics_fillCircle(g_sContext_p, 90, 70, 4);
}

void WriteSpeed(Speeds *speed, Graphics_Context *g_sContext_p){

    if (speed->Vx == 0){
        //Graphics_clearDisplay(g_sContext_p);
        char text[25] = "";
        Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_WHITE);
        speed->Vx = 0;
        sprintf(text, "%u", speed->Vx);
        Graphics_drawString(g_sContext_p, (int8_t *) text, -1, 75, 4, true);

    }
}

void DrawVxVy(Graphics_Context *g_sContext_p){                                                              //This will draw Vx and Vy respectively on the screen
    char string[10] = "Vx:";
    char string2[10]= "Vy:";
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_WHITE);
    Graphics_drawString(g_sContext_p,(int8_t *) string , -1, 50, 4, true);                                             //Draw Vx string on top of screen
    Graphics_drawString(g_sContext_p,(int8_t *) string2 , -1, 50, 14, true);                                            //Draw Vy string on top of screen
}


bool printList_nonBlocking(Graphics_Context *g_sContext_p, int n)
{

    // Since this is the only function using this enum, there is no need to create a type for it.
    // However, if you like you can do it.
    static enum {INIT, CHECKING, WAITING} state = INIT;

    int8_t string[2];
    string[1] = 0;

    // The n is the maximum we should print, we just make sure n is not more than 9 (single digit)
    if (n>9)
        n = 9;

    // x is static as we have to remember where was the last place we wrote to
    static int x = 10;

    // i is static as we have to remember how far in the list we have printed
    static int i;

    // timer has to be static so that we remember how long it has passes since we wrote the last number
    static OneShotSWTimer_t waitTimer;

    bool do_print = false;
    switch(state)
    {
        // This function is too simple and it really does not need a separate state for init.
        // However, to show you how most functions work, I show you the init in a separate state
        case INIT:
            // We usually don't put functions inside the FSM, however, this is a function fully tested and trusted, so it's OK
            InitOneShotSWTimer(&waitTimer, &timer0, BLOCKING_WAIT_TIME);
            x = 10;
            i = 0;
            Graphics_drawString(g_sContext_p,"                              ", -1, x, 5, true);

            // from INIT, we are printing at least the 0, therefore do_print is 1 and we go to waiting from here
            do_print = true;
            state = WAITING;
            break;

        case CHECKING:
            // If i is passed n, we have finished printing the list and
            if (i>n) {

                // Since we are done with this list, our next time we enter the function, we are initializing working with a new list
                state = INIT;

                //we return "done" signal as true
                return true;

            }
            else
            {

                // If we are not done yet, we have to print another character and go to waiting
                do_print = true;
                state = WAITING;
            }

            break;

        case WAITING:
            if (OneShotSWTimerExpired(&waitTimer))
                state = CHECKING;
            break;

    }


    if (do_print)
    {
        string[0] = i+'0';
        Graphics_drawString(g_sContext_p,string , -1, x, 5, true);
        StartOneShotSWTimer(&waitTimer);
        x = x + 10;
        i++;
    }

    return false;
}





void make_5digit_NumString(unsigned int num, int8_t *string)
{
    string[0]= (        num  / 10000) +'0';
    string[1]= ((num%10000) / 1000) +'0';
    string[2]= ((num%1000) / 100) +'0';
    string[3]= ((num%100) / 10) +'0';
    string[4]= ((num%10) / 1) +'0';
    string[5]= 0;
}

void drawXY(Graphics_Context *g_sContext_p, unsigned int x, unsigned int y)
{
    int8_t string[6];

    Graphics_drawString(g_sContext_p, "x=", -1, 10, 5, true);
    make_5digit_NumString(x, string);
    Graphics_drawString(g_sContext_p, string, -1, 30, 5, true);

    Graphics_drawString(g_sContext_p, "y=", -1, 10, 15, true);
    make_5digit_NumString(y, string);
    Graphics_drawString(g_sContext_p, string, -1, 30, 15, true);
}


void draw_Base(Graphics_Context *g_sContext_p)
{
    Graphics_Rectangle R;
    R.xMin = 0;
    R.xMax = 127;
    R.yMin = 32;
    R.yMax = 96;

    Graphics_drawRectangle(g_sContext_p, &R);
    Graphics_fillCircle(g_sContext_p, 63, 63, 10);
    Graphics_drawString(g_sContext_p, "circle move #:", -1, 10, 100, false);
    Graphics_drawString(g_sContext_p, "000", -1, 10, 110, true);
}

void make_3digit_NumString(unsigned int num, int8_t *string)
{
    string[0]= (num/100)+'0';
    string[1]= ((num%100) / 10) + '0';
    string[2]= (num%10)+'0';
    string[3] =0;

}


void MoveCircle(Graphics_Context *g_sContext_p, bool moveToLeft, bool moveToRight)
{
    static unsigned int x = 63;
    static unsigned int y = 63;
    static bool moveToDown = true;

    static unsigned int moveCount = 0;
    int8_t string[4];

    static OneShotSWTimer_t yMoveTimer;

    static bool init = true;
    if (init)
    {
        //
        InitOneShotSWTimer(&yMoveTimer,
                           &timer0,
                           BALL_TIME_STEP);
        StartOneShotSWTimer(&yMoveTimer);

        init = false;
    }

    if (OneShotSWTimerExpired(&yMoveTimer))
    {
        Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_BLUE);
        Graphics_fillCircle(g_sContext_p, x, y, 10);

        StartOneShotSWTimer(&yMoveTimer);
        if (moveToDown)
        {
            y = y + BALL_Y_STEP;
            if (y > 80)
                moveToDown = false;
        }
        else
        {
            y = y - BALL_Y_STEP;
            if (y < 50)
                moveToDown = true;
        }

        Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_YELLOW);
        Graphics_fillCircle(g_sContext_p, x, y, 10);
    }

    if ((moveToLeft && (x>20)) || (moveToRight && (x<110)))
    {
        Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_BLUE);
        Graphics_fillCircle(g_sContext_p, x, y, 10);

        if (moveToLeft)
            x = x-10;
        else
            x = x+10;

        Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_YELLOW);
        Graphics_fillCircle(g_sContext_p, x, y, 10);

        moveCount++;
        make_3digit_NumString(moveCount, string);
        Graphics_drawString(g_sContext_p, string, -1, 10, 110, true);
    }

}
void InitFonts() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
}


void InitGraphics(Graphics_Context *g_sContext_p) {

    Graphics_initContext(g_sContext_p,
                         &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(g_sContext_p, GRAPHICS_COLOR_YELLOW);
    Graphics_setBackgroundColor(g_sContext_p, GRAPHICS_COLOR_BLUE);
    Graphics_setFont(g_sContext_p, &g_sFontCmtt12);

    InitFonts();

    Graphics_clearDisplay(g_sContext_p);
}


void drawAccelData(Graphics_Context *g_sContext_p, uint16_t *resultsBuffer)
{
    int8_t string[6];
    make_5digit_NumString(resultsBuffer[0], string);
    Graphics_drawStringCentered(g_sContext_p,
                                    (int8_t *)string,
                                    8,
                                    64,
                                    30,
                                    OPAQUE_TEXT);

    make_5digit_NumString(resultsBuffer[1], string);
    Graphics_drawStringCentered(g_sContext_p,
                                    (int8_t *)string,
                                    8,
                                    64,
                                    42,
                                    OPAQUE_TEXT);

    make_5digit_NumString(resultsBuffer[2], string);
    Graphics_drawStringCentered(g_sContext_p,
                                    (int8_t *)string,
                                    8,
                                    64,
                                    62,
                                    OPAQUE_TEXT);
}
