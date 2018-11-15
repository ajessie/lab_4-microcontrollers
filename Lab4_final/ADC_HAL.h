/*
 * ADC_HAL.h
 *
 *  Created on: Oct 18, 2018
 *      Author: Leyla
 */

#ifndef ADC_HAL_H_
#define ADC_HAL_H_

void initADC_accelerometer();
void initADC();
void initADC_INT();
void startADC();
void initJoyStick();
void initAccelerometer();
void getSampleJoyStick(unsigned *X, unsigned *Y);
void getSampleAccelerometer(uint16_t *resultsBuffer);


#endif /* ADC_HAL_H_ */
