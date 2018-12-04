#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "ButtonLED_HAL.h"
#include "graphics_HAL.h"
#include "ADC_HAL.h"
#include "labyrinth.h"


extern HWTimer_t timer0, timer1;

/* ADC results buffer */

// This function initializes all the peripherals except graphics
void initialize();
void ModifyLEDColor(bool leftButtonWasPushed, bool rightButtonWasPushed);


#define LEFT_THRESHOLD  0x1000
#define STABLE_THRESHOLD 0x1DBF
#define VX_ZERO_GEAR    0x19BD
#define ONE_LEFT_TILT 0x1BFF
#define DURATION 100
#define VX_LEFT_TILT_THREE 0x13F1
#define MOVE_RIGHT 0x1C91
#define STABLE_X 0x1C0C
#define STABLE_Y 0x1C84
#define MOVE_RIGHT_SLOW_X 0x1CE8
#define MOVE_RIGHT_SLOW_Y 0x1EDC
#define MOVE_LEFT_SLOW_X  0x18CE
#define MOVE_LEFT_SLOW_X2 0x1BBC
#define MOVE_DOWN_SLOW_Y 0x1B58
#define MOVE_DOWN_SLOW_Y2 0x189C
#define MOVE_UP_SLOW_Y 0x1D24
#define MOVE_UP_SLOW_Y2 0x1F40

int main(void)
 {
    Graphics_Context g_sContext;
    Speeds speed;
    marble_t marble;
    hole_t hole;
    InitGraphics(&g_sContext);

    initialize();

    button_t LauchpadLeftButton;
    initButton(&LauchpadLeftButton, GPIO_PORT_P1, GPIO_PIN1, &timer0);

    button_t LauchpadRightButton;
    initButton(&LauchpadRightButton, GPIO_PORT_P1, GPIO_PIN4, &timer0);


    uint16_t resultsBuffer[3];


    OneShotSWTimer_t OST;

    InitOneShotSWTimer(&OST,
                       &timer0,
                       100000);
    StartOneShotSWTimer(&OST);
    DrawBall(&g_sContext, &marble);
    TargetCell(&g_sContext, &marble, &speed);

    while (1)
    {
        if (OneShotSWTimerExpired(&OST)) {

            getSampleAccelerometer(resultsBuffer);                                              //Get movement from tilt of board

            if (resultsBuffer[0] > STABLE_X && resultsBuffer[1] < STABLE_Y){                    //handle stable ball movement
                speed.Vx = 0;
                speed.Vy = 0;
                marble.direction = Stable;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);
            }

            if (resultsBuffer[0] > MOVE_RIGHT_SLOW_X && resultsBuffer[0] < MOVE_RIGHT_SLOW_Y){  //slow movement to the right gear one
                speed.Vx = 1;
                speed.Vy = 0;
                marble.direction = Right;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);
            }

            if (resultsBuffer[0] > MOVE_RIGHT_SLOW_Y){                                          //switch to 2, or gear 2
                speed.Vx = 2;
                speed.Vy = 0;
                marble.direction = Right;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);
            }

            if (resultsBuffer[0] > MOVE_LEFT_SLOW_X && resultsBuffer[0] < MOVE_LEFT_SLOW_X2){   //move left at 1
                speed.Vx = 1;
                speed.Vy = 0;
                marble.direction = Left;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);
            }

            if(resultsBuffer[0] < MOVE_LEFT_SLOW_X){                                            //move left at 2
                speed.Vx = 2;
                speed.Vy = 0;
                marble.direction = Left;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);
            }

            if(resultsBuffer[1] < MOVE_DOWN_SLOW_Y && resultsBuffer[1] > MOVE_DOWN_SLOW_Y2){  //move down at speed 1
                speed.Vx = 0;
                speed.Vy = 1;
                marble.direction = Down;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);
            }

            if (resultsBuffer[1] < MOVE_DOWN_SLOW_Y2){                                         //move down at speed 2
                speed.Vx = 0;
                speed.Vy = 2;
                marble.direction = Down;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);
            }

            if (resultsBuffer[1] > MOVE_UP_SLOW_Y && resultsBuffer[1] < MOVE_UP_SLOW_Y2){     //Move up at speed 1
                speed.Vx = 0;
                speed.Vy = 1;
                marble.direction = Up;
                WriteSpeed(&speed, &g_sContext);
                MoveBall(&g_sContext, &marble, &speed);

            }

            if (resultsBuffer[1] > MOVE_UP_SLOW_Y2 ){                                       //Move up at speed 2
               speed.Vx = 0;
               speed.Vy = 2;
               marble.direction = Up;
               WriteSpeed(&speed, &g_sContext);
               MoveBall(&g_sContext, &marble, &speed);
           }

            StartOneShotSWTimer(&OST);
        }
       // MoveBall(&g_sContext, &marble, &speed);
        bool leftButtonPushed = ButtonPushed(&LauchpadLeftButton);
        bool rightButtonPushed = ButtonPushed(&LauchpadRightButton);

        if (LaunchpadLeftButton_pressed())
            turnOn_LaunchpadLED1();
        else
            turnOff_LaunchpadLED1();

        if (LaunchpadRightButton_pressed())
            turnOn_LaunchpadLED2_red();
        else
            turnOff_LaunchpadLED2_red();


        ModifyLEDColor(leftButtonPushed,rightButtonPushed);
        Win(&g_sContext, &marble, &hole);                   //see if the player won
        GameOver(&g_sContext, &marble, &speed);             //game over
        DrawEasyStage(&g_sContext, &marble, &speed);        //draw the maze
        DrawWalls(&g_sContext);                             //draw boundary
        DrawVxVy(&g_sContext);                              //display X and Y
    }
}


void initialize()
{
    // stop the watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // initialize the boosterPack LEDs and turn them off except for red LED
    initialize_BoosterpackLED_red();
    initialize_BoosterpackLED_green();
    initialize_BoosterpackLED_blue();
    initialize_LaunchpadLED1();
    initialize_LaunchpadLED2_red();
    initialize_LaunchpadLED2_blue();
    initialize_LaunchpadLED2_green();

    turnOff_BoosterpackLED_red();
    turnOff_BoosterpackLED_green();
    turnOff_BoosterpackLED_blue();
    turnOff_LaunchpadLED1();
    turnOff_LaunchpadLED2_red();
    turnOff_LaunchpadLED2_blue();
    turnOff_LaunchpadLED2_green();

    initHWTimer0();
    initHWTimer1();


    initAccelerometer();
    initADC();
    startADC();
}

// This FSM has two inputs each of them the FSM if a button has been pushed or not
// The FSM has three states: Red, Green, Blue. The initial state is Red
// The FSM has three outputs, each output is a boolean that decides if an LED should be on or off
// When the left button is pressed, the FSM goes
void ModifyLEDColor(bool leftButtonWasPushed, bool rightButtonWasPushed)
{
    typedef enum {red, green, blue} LED_state_t;

    static LED_state_t BoosterLED = red;

    // outputs of the FSM and their default
    bool toggleGreen = false;
    bool toggleBlue = false;
    bool toggleRed = false;

    switch(BoosterLED)
    {
    case red:
        if (leftButtonWasPushed)
        {
            // next state
            BoosterLED = green;

            //outputs
            // This turns green on
            toggleGreen = true;

            // This turns red off
            toggleRed = true;
        }
        else if (rightButtonWasPushed)
        {
            BoosterLED = blue;

            //outputs
            toggleBlue = true;
            toggleRed = true;
        }
        break;
    case green:
        if (leftButtonWasPushed)
        {
            // next state
            BoosterLED = blue;

            //outputs
            toggleBlue = true;
            toggleGreen = true;
        }
        else if (rightButtonWasPushed)
        {
            BoosterLED = red;

            //outputs
            toggleRed = true;
            toggleGreen = true;
        }
        break;
    case blue:
        if (leftButtonWasPushed)
        {
            // next state
            BoosterLED = red;

            //outputs
            toggleRed = true;
            toggleBlue = true;
        }
        else if (rightButtonWasPushed)
        {
            BoosterLED = green;

            //outputs
            toggleGreen = true;
            toggleBlue = true;
        }
    }

    if (toggleRed)
        toggle_BoosterpackLED_red();

    if (toggleGreen)
        toggle_BoosterpackLED_green();

    if (toggleBlue)
        toggle_BoosterpackLED_blue();

}


/* This interrupt is fired whenever a conversion is completed and placed in
 * ADC_MEM2. This signals the end of conversion and the results array is
 * grabbed and placed in resultsBuffer */
/*void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    if(status & ADC_INT2)
    {
        resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
        resultsBuffer[2] = ADC14_getResult(ADC_MEM2);

    }


}*/

