/*
 * mylogic.c
 *
 *  Created on: Apr 29, 2020
 *      Author: pkda001
 */

/* DriverLib Includes */
#include <behavior.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include <string.h>

/* User-developed modules */
#include "display.h"
#include "keypad.h"
#include "system.h"
#include "temp_sensor.h"
#include "thermal_source.h"
#include "behavior.h"
#include "timer.h"

/*
 * ======================================
 * THE VALUES BELOW ARE DEFINED IN main.c
 * ======================================
 */

/* The temperature targets for the various phases in fixed point
 * for fixed point (with one decimal place) YYY => YY.Y e.g 652 => 65.2 C
 *
 * According to the sources the target temperature are:
 *      Phase 1: usually between 90 and 100
 *      Phase 2: usually between 30 and 60
 *      Phase 3: usually between 60 and 70
 *      Phase 4: (optional), usually same temperature as Phase 3
 *
 * (need 16-bit number to hold values > 256)
 */
extern uint16_t phaseTargetTemp[NUM_PHASES];

/* The temperature targets threshold/tolerance in fixed point */
extern uint16_t tempTolerance;

/* The length of each phase in SECONDS
 *
 * According to sources the target lengths are:
 *
 *      Phase 1: usually   1 - 3 minutes (for first cycle)
 *               usually 0.5 - 2 minutes (for subsequent cycles)
 *      Phase 2: usually 0.5 - 2 minutes
 *      Phase 3: depends on enzyme used and length of sample
 *               rule of thumb is:
 *               1 min/kb for fast enzyme
 *               2 min/kb for slow enzyme
 *
 *               Make assumptions about enzyme and sample length (kb) and pick
 *               a number.
 *
 *      Phase 4: usually  5 - 15 minutes
 *               since this is a simulation, you can pick something less.
 *
 */
extern uint16_t phaseTargetTime[NUM_PHASES];

/*
 * Number of times to cycle the thermocycler
 */
extern uint16_t numCycles;

/*
 * This is the file where you implement your own behavior
 *
 * YOU MUST AT LEAST IMPLEMENT THE FOLLOWING FUNCTIONS
 *
 *      void runThermalCyclerBehavior(bool keypadOn)
 *          This is where some of the main logic of the microwave runs.
 *
 *      void Timer_handleTimeout(void)
 *          This will be called every second if the timer has been started.
 *
 */
int i = -1;
int j;
int p = 3;
int stop = 1;
int Timer32_1;
void runThermalCyclerBehavior(bool keypadOn)
{
    if(stop == 1 && p == 2)
    {
        stop = 0;
        timer_start();
        Timer32_1 = 300;
        ThermalSource_turnOff();
        Timer_stop();
    }
    if(stop == 0 && p == 1 && TempSensor_getTemperature()<689)
    {
        p = 2;
        timer_start();
        Timer32_1 = 90;
        ThermalSource_turnOff();
    }
    if(stop == 0 && p == 0 && TempSensor_getTemperature()>545)
    {
        p = 1;
        timer_start();
        Timer32_1 = 15;
        ThermalSource_turnOff();
    }
    if(stop == 0 && p == 3 && TempSensor_getTemperature()<943)
    {
        p = 0;
        timer_start();
        Timer32_1 = 15;
        ThermalSource_turnOff();
    }
    if(stop == 1 && p == 3)
    {
        if(i == 3)
        {
            p = -1;
            stop = -1;
        }
        else
        {
            i++;
            stop = 0;
            ThermalSource_turnHeatOn();
        }
    }
    if(stop == 1 && p == 0)
    {
        ThermalSource_turnCoolOn();
        stop = 0;
    }
    if(stop == 1 && p == 1)
    {
        stop = 0;
        ThermalSource_turnHeatOn();
    }
}

void Timer_handleTimeout(void)
{
    /*Professor, you did not write any code for keypad.
     * In this case, I can't write keyboard interrupt.
     * But if you write something, I will do this:
     * if ((interrupt) == true)
     *      Timer_handleTimeout(void)*/
    Timer_clearInterruptFlag();
    if(Timer32_1 == 0)
    {
        Timer_stop();
        stop = 1;
    }
    Timer32_1 = Timer32_1 - 1;
    Display_writeString(0,2,"CYCLE");
    Display_writeString(0,8,"PHASE");
    Display_writeString(0,14,"TIME");
    char Cstr[1];
    /*Display_writeString(1,4,itoa(i,Cstr,10));*/
    char Pstr[1];
    /*Display_writeString(1,10,itoa(p,Pstr,10));*/
    char Mstr[1],Sstr[1];
    /*Display_writeString(1,14,itoa(j/60,Mstr,10));*/
    Display_writeString(1,15,":");
    /*Display_writeString(1,16,itoa(j%60,Pstr,10));*/
    Display_writeString(2,0,"  ");
    Display_writeString(2,6,"     ");
    Display_writeString(2,2,"TEMP");
    Display_writeString(2,13,"TARGET");
    char t2[2];
    itoa(TempSensor_getTemperature()/10,t2,10);
    /*Display_writeString(3,2,t2);*/
    char t1[1];
    Display_writeString(3,4,".");
    itoa(TempSensor_getTemperature()%10,t1,10);
    /*Display_writeString(3,5,t1);*/
    /*Display_writeString(3,13,phaseTargetTemp[p]/10);*/
    Display_writeString(3,15,".");
    char t3[2];
    itoa(Timer32_1,t3,10);
    /*Display_writeString(3,16,phaseTargetTemp[p]%10);*/
    Display_writeString(0,0,t3);
}
