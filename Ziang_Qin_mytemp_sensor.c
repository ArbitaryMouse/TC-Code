/*
 * mytemp_sensor.c
 *
 *  Created on: May 1, 2020
 *      Author: pkda001, Ziang Qin
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Module includes */
#include "uart.h"
#include "temp_sensor.h"

/* You can provide implementations for any of the functions in temp_sensor.h
 *
 * The simplest ones are
 *      bool TempSensor_newMessage(void)
 *      void TempSensor_setup(void)
 *
 * A simple non-trivial one is TempSensor_getTemperature.
 *      Inside that function you need to get the raw ADC value and then
 *      process it.
 *
 * The most complex one is uint16_t TempSensor_getRawADCValue(void)
 *      You have to
 *      1. Read the data from the UART
 *      2. Find the ADC values (they look like /YY/ where each y is
 *         an 8-bit unsigned number. The two numbers form a 16-bit
 *         unsigned number
 *      3. convert the two 8-bit numbers into the 16-bit number
 *      4. Return the 16-bit number.
 */

bool TempSensor_newMessage(void){
    int msg = 0;
    return UART_getMessage(msg) != 0;
}

uint16_t TempSensor_getTemperature(void){
    int raw;
    raw = TempSensor_getRawADCValue();
    float result;
    result = (raw/65536*3.3-1.65)*1000;
    return result;
}

uint16_t TempSensor_getRawADCValue(void){
    int msg;
    uint16_t raw;
    raw = UART_getMessage(msg);
    uint8_t n1,n2;
    n1 = msg / 256;
    n2 = msg % 256;
    int final;
    final = n1*256+n2;
    return final;
}
