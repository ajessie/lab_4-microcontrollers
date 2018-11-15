/*
 * ButtonLED_HAL.h
 *
 *  Created on: Sep 15, 2018
 *      Author: Leyla
 */

#ifndef BUTTONLED_HAL_H_
#define BUTTONLED_HAL_H_

#include "Timer32_HAL.h"

// A new type that enumerates the debouncing states of a button
// These will be used for debouncing FSM
typedef enum {Stable_P, Tran_PtoR, Stable_R, Tran_RtoP} debounce_state_t;

// A new type that enumerates the pushing states of a button
// These will be used for push-button FSM
typedef enum {PRESSED_STATE, RELEASED_STATE} push_button_state_t;

// A new struct that keeps info about a button using its port and pin numbers
typedef struct
{
    uint_fast8_t Port; //port of the button GPIO
    uint_fast16_t Pin; //pin of the button GPIO
    debounce_state_t Debounce_state; //The state that keeps track of button's debouncing state
    push_button_state_t PushButton_state; // The state that keeps track of button being pushed
    OneShotSWTimer_t timer; // The timer used for debouncing the button
} button_t;


void initialize_LaunchpadLED1();
void initialize_LaunchpadLED2_red();
void initialize_LaunchpadLED2_green();
void initialize_LaunchpadLED2_blue();
void initialize_BoosterpackLED_red();
void initialize_BoosterpackLED_green();
void initialize_BoosterpackLED_blue();


void turnOn_LaunchpadLED1();
void turnOn_LaunchpadLED2_red();
void turnOn_LaunchpadLED2_green();
void turnOn_LaunchpadLED2_blue();
void turnOn_BoosterpackLED_red();
void turnOn_BoosterpackLED_green();
void turnOn_BoosterpackLED_blue();

void turnOff_LaunchpadLED1();
void turnOff_LaunchpadLED2_red();
void turnOff_LaunchpadLED2_green();
void turnOff_LaunchpadLED2_blue();
void turnOff_BoosterpackLED_red();
void turnOff_BoosterpackLED_green();
void turnOff_BoosterpackLED_blue();

void toggle_LaunchpadLED1();
void toggle_LaunchpadLED2_red();
void toggle_LaunchpadLED2_green();
void toggle_LaunchpadLED2_blue();
void toggle_BoosterpackLED_red();
void toggle_BoosterpackLED_green();
void toggle_BoosterpackLED_blue();

void initialize_LaunchpadLeftButton();
void initialize_LaunchpadRightButton();

void initialize_BoosterpackTopButton();
void initialize_BoosterpackBottomButton();

char switchStatus_LaunchpadLeft();
char switchStatus_LaunchpadRight();
char switchStatus_BoosterpackTop();
char switchStatus_BoosterpackBottom();

char LaunchpadLeftButton_pressed();
char LaunchpadRightButton_pressed();
char BoosterpackTopButton_pressed();
char BoosterpackBottomButton_pressed();

bool ButtonPushed(button_t *button);

void initButton(button_t *button_p, uint_fast8_t Port, uint_fast16_t Pin,
                HWTimer_t* hwtimer_p);

#endif /* BUTTONLED_HAL_H_ */
