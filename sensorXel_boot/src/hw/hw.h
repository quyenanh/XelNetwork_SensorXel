/*
 * hw.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef HW_H_
#define HW_H_


#ifdef __cplusplus
 extern "C" {
#endif



#include "hw_def.h"
#include "bsp.h"
#include "qbuffer.h"
#include "led.h"
#include "button.h"
#include "mux.h"
#include "timer.h"
#include "uart.h"
#include "dxlport.h"
#include "eeprom.h"
#include "flash.h"
#include "reset.h"


void hwInit(void);

void delay(uint32_t delay_ms);
void delayMillis(uint32_t delay_ms);
void delayMicros(uint32_t delay_us);
void delaySeconds(uint32_t delay_sec);

uint32_t millis(void);
uint32_t micros(void);




#ifdef __cplusplus
}
#endif


#endif /* HW_H_ */
