#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "ButtonLED_HAL.h"
#include "graphics_HAL.h"
#include "ADC_HAL.h"

extern HWTimer_t timer0, timer1;


//typedef enum {zero,one,two} gears;
//
//typedef struct Speeds
//{
//    gears Speed;
//    int Vx;
//    int Vy;
//} Speeds;
//
//static Speeds speed = zero;

/* ADC results buffer */

// This function initializes all the peripherals except graphics
void initialize();
void ModifyLEDColor(bool leftButtonWasPushed, bool rightButtonWasPushed);


#define LEFT_THRESHOLD  0x1000
#define STABLE_THRESHOLD 0x1DBF
#define VX_ZERO_GEAR    0x19BD
#define DURATION 100


int main(void)
{
    Graphics_Context g_sContext;
    Speeds speed;
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

    while (1)
    {
        if (OneShotSWTimerExpired(&OST)) {

            getSampleAccelerometer(resultsBuffer);
//            GearShift()

//            if (resultsBuffer[0] < STABLE_THRESHOLD){
//                    DrawBall(&g_sContext);                                //Draw the ball here
//                    speed.Speed = zero;
//                    //DrawVxVy(&g_sContext);                              //Draw Vx and Vy here
//            }

            if (resultsBuffer[0] < VX_ZERO_GEAR ){
                 speed.Vx = 0;
                 WriteSpeed(&speed, &g_sContext);
            }

//           WriteSpeed(&speed, &g_sContext);
           // drawAccelData(&g_sContext, resultsBuffer);

            StartOneShotSWTimer(&OST);

        }


        //GearShift(resultsBuffer, &speed, &g_sContext);
        //WriteSpeed(&speed, &g_sContext);
       // WriteSpeed(&speed, &g_sContext);
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
        DrawBall(&g_sContext);
        //DrawTop(&g_sContext);
        DrawWalls(&g_sContext);
        DrawEasyStage(&g_sContext);
        DrawVxVy(&g_sContext);
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

    turnOn_BoosterpackLED_red();
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

