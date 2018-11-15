/*
 *
 *  Created on: Sep 14, 2018
 *      Author: akhilr
 *      Modified: LN
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "ButtonLED_HAL.h"



// According to the schematics on page 37 of the Launchpad user guide,
// When a button is pressed, it is grounded (logic 0)
#define PRESSED 0
#define RELEASED 1

// The debounce time is 100ms which is 100,000 us
#define DEBOUNCE_TIME_IN_US 100000


void initialize_LaunchpadLED1()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
}

void initialize_LaunchpadLED2_red()
{
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
}

void initialize_LaunchpadLED2_green()
{
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
}

void initialize_LaunchpadLED2_blue()
{
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);
}

void initialize_BoosterpackLED_red()
{
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);
}

void initialize_BoosterpackLED_green()
{
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);
}

void initialize_BoosterpackLED_blue()
{
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6);
}

void turnOn_LaunchpadLED1()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void turnOn_LaunchpadLED2_red()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

void turnOn_LaunchpadLED2_green()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void turnOn_LaunchpadLED2_blue()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

void turnOn_BoosterpackLED_red()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

void turnOn_BoosterpackLED_green()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
}

void turnOn_BoosterpackLED_blue()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);
}

void turnOff_LaunchpadLED1()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void turnOff_LaunchpadLED2_red()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

void turnOff_LaunchpadLED2_green()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void turnOff_LaunchpadLED2_blue()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

void turnOff_BoosterpackLED_red()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

void turnOff_BoosterpackLED_green()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
}

void turnOff_BoosterpackLED_blue()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);
}

void toggle_LaunchpadLED1()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void toggle_LaunchpadLED2_red()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

void toggle_LaunchpadLED2_green()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void toggle_LaunchpadLED2_blue()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
}

void toggle_BoosterpackLED_red()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

void toggle_BoosterpackLED_green()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
}

void toggle_BoosterpackLED_blue()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN6);
}

void initialize_LaunchpadLeftButton()
{
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

void initialize_LaunchpadRightButton()
{
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);

}

void initialize_BoosterpackTopButton()
{
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);
}

void initialize_BoosterpackBottomButton()
{
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);
}

char switchStatus_LaunchpadLeft()
{
    return (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1));
}

char switchStatus_LaunchpadRight()
{
    return (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4));
}

char switchStatus_BoosterpackTop()
{
    return (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1));
}

char switchStatus_BoosterpackBottom()
{
    return (GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5));
}



char LaunchpadLeftButton_pressed()
{
    return ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1)) == PRESSED);
}
char LaunchpadRightButton_pressed()
{
    return ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4)) == PRESSED);
}
char BoosterpackTopButton_pressed()
{
    return ((GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN1)) == PRESSED);
}

char BoosterpackBottomButton_pressed()
{
    return ((GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN5)) == PRESSED);
}


//------------------------------------------
// Debounce FSM
// This FSM simply debounces the button status, it acts as a filter that removes the undesired bounces
// This FSM has two inputs the raw button status (rawButtonStatus). The input struct holds info on which port/pin to check.
// The other input is the status of the timer. The input struct hold info about which timer to check.

// The FSM also has two outputs. One is the debounced button status (debouncedButtonStatus), which is also the output of this function
// The other output is a boolean that decides whether to start the timer. This output of the FSM is internal to the function.

char Debounce(button_t *button) {

    // The first input of the FSM
    char rawButtonStatus = GPIO_getInputPinValue(button->Port, button->Pin);

    // The second input of the FSM
    bool timerExpired = OneShotSWTimerExpired(&(button->timer));

    // outputs of the FSM
    bool debouncedButtonStatus;
    bool startTimer = false;

    // The button struct holds info on the debouncing state of the button
    // e.g. it knows if the button is in transition or not
    switch (button->Debounce_state)
    {
    case Stable_P:
        // The output of both arcs is the same so we can put this statement outside the transition arcs
        debouncedButtonStatus = PRESSED;
        if (rawButtonStatus != PRESSED)
        {
            // Change state
            button->Debounce_state = Tran_PtoR;

            // Update outputs, if different from default
            startTimer = true;

         }
        break;

    case Tran_PtoR:
        // The output of all three arcs is the same so we can put this statement outside the transition arcs
        debouncedButtonStatus = PRESSED;

        if (rawButtonStatus == PRESSED)
            button->Debounce_state = Stable_P;
        else
            if (timerExpired)
                //Change state
                button->Debounce_state = Stable_R;

        break;

    case Stable_R:
        // The output of both arcs is the same so we can put this statement outside the transition arcs
        debouncedButtonStatus = RELEASED;

        if (rawButtonStatus == PRESSED) {
            // Change state
            button->Debounce_state = Tran_RtoP;

            // Update outputs, if different from default
            startTimer = true;
        }
        break;

     case Tran_RtoP:
         debouncedButtonStatus = RELEASED;

         if (rawButtonStatus == RELEASED)
             button->Debounce_state = Stable_R;
         else
             if (timerExpired)
                 button->Debounce_state = Stable_P;
        break;
    }

    if (startTimer)
        StartOneShotSWTimer(&(button->timer));

    return debouncedButtonStatus;
}

// This FSM implements the behavior of a push-button
// We take action (e.g. toggle light) only when the button goes from being released to pressed.
// No action is done when the button is kept pressed or kept released.
// No action is done when the button goes from pressed to released either.
// In short, we take action only once in a full circle of button going from released->pressed->released

bool ButtonPushed(button_t *button)
{
    // input to the FSM
    char debouncedButtonStatus = Debounce(button);

    // The output of the FSM
    // To make the function brief and more efficient, we can assign a default value to the output
    bool take_action = false;

    switch(button->PushButton_state)
    {
    // make sure you have one "case" for each state.
    // make sure there is a break at the end of each case before the next case
    case PRESSED_STATE:
        // for each state, make sure you implement each transition arc

        // transition arc from PRESSED_STATE to RELEASED_STATE
        if (debouncedButtonStatus == RELEASED)
        {
            // assign the output
            // this is not needed anymore: take_action = false;

            // next state
            button->PushButton_state = RELEASED_STATE;
         }
        // The arc from PUSHED to PUSHED does not need implementation since the state does not change AND its output is also defaul

        break;


    case RELEASED_STATE:

        // transition arc from RELEASED to PUSHED
        if (debouncedButtonStatus == PRESSED)
        {
            //output of the arc
            take_action = true;

            // next state
            button->PushButton_state = PRESSED_STATE;
        }
        // arc from RELEASED to RELEASED does not need implementation for a similar reason to line 172

     }


    return(take_action);

}


void initButton(button_t *button_p, uint_fast8_t Port, uint_fast16_t Pin, HWTimer_t* hwtimer_p)
{
    button_p->Debounce_state = Stable_R;
    button_p->PushButton_state = RELEASED_STATE;
    button_p->Port = Port;
    button_p->Pin = Pin;
    InitOneShotSWTimer(&(button_p->timer), hwtimer_p, DEBOUNCE_TIME_IN_US);
    GPIO_setAsInputPinWithPullUpResistor(button_p->Port, button_p->Pin);
}
