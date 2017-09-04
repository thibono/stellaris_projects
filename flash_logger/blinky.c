
 /* Flash datalogger
  * this code separates the flash memory in two parts: code and data.
  * the data region starts in the address 0x10000
  * the register 0xFFE4 holds the last address used to store data in the flash.
  * for example, in case of a reset, it starts storing data from the last position.
  *
  * this is a very simple example where the array "coffee is good!!" is stored on memory.
  * to maximize the use of the 32bit memory, I converted 4 uint8_t bytes into one 32 word.
  * Then, I stored that word in the memory.
  *
  *
  *
*/

/*
 *
 * This code is a modification from sample codes available from texas instruments.
 * here is the Copyright:
 *
 Copyright (c) 2012-2016 Texas Instruments Incorporated.  All rights reserved.
 Software License Agreement

 Texas Instruments (TI) is supplying this software for use solely and
 exclusively on TI's microcontroller products. The software is owned by
 TI and/or its suppliers, and is protected under applicable copyright
 laws. You may not combine this software with "viral" open-source
 software in order to form a larger program.

 THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
 NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
 NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
 CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
 DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/flash.h"


#define DATALOG_MEMORY_START    (0X10000)
#define DATALOG_MEMORY_END      (0X40000)

// Store data in buffer before writing to flash memory
#define DATALOG_BUFFER_SIZE             (128u)
#define DATALOG_BUFFER_WRITE_THRESHOLD  (100u)

// Flags
typedef union
{
    struct
    {
        uint8_t on          : 1; // 1 = data logging has started
        uint8_t memory_full : 1; // 1 = memory is full
        uint8_t one_second  : 1; // 1 = 1 second has elapsed
    } flag;
    uint8_t all;                 // Shortcut to all flags (for reset)
} datalog_flags;

struct datalog
{
    // Flags
    datalog_flags flags;

    // Data logging mode
    uint8_t mode;

    // Data logging interval
    uint8_t interval;

    // Data logging delay counter
    uint32_t delay;

    // Datalog memory write pointer
    uint32_t *wptr;

    // Datalogger write buffer index
    uint8_t idx;

    // Datalogger write buffer
    uint32_t buffer[DATALOG_BUFFER_SIZE];
};
extern struct datalog sDatalog;

// Global Variable section
struct datalog sDatalog;

// *************************************************************************************************
// Write buffer content to flash
// adapted from ez-chronos
// Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
// *************************************************************************************************
void datalog_write_buffer(void)
{
    uint8_t i = 0;
    uint32_t data[1];
    volatile uint32_t temp;

    // Write buffer content to flash
    while (i < sDatalog.idx - 1)
    {
        // Keep array order when writing to flash memory
        data[0] = sDatalog.buffer[i++];
        data[0] += (uint32_t) (sDatalog.buffer[i++] << 8);
        data[0] += (uint32_t) (sDatalog.buffer[i++] << 16);
        data[0] += (uint32_t) (sDatalog.buffer[i++] << 24);

      // Write 32-bit word to flash
         FlashProgram(data, sDatalog.wptr++, sizeof(data));


    }
    //stores the last memory logger position in the memory
    FlashErase(0xffe4);
    FlashProgram(&(sDatalog.wptr), 0xffe4, 4);

}


int main(void)
{
     uint32_t data[1];
    int j;

    // I am storing the most recent flash memory address in the Flash.
    // by doing that, every time you get a reset the pointer continues from where it stoped.
    // the address i 0xffe4.
    // to get the value of this mem. add. you need to do: *( uint32_t *)(ADRESSS)
    // if the value is equal to 0xffffffff, it means you just compiled your code and the whole flash is reseted
    // from here, I load the datalog memory start point, which is the address 0x10000
    // If the value is not 0xfffff..., then load the previous register.
    // it works well.
    // obs: because sDatalog.wptr is a pointer, I want to store the address of that pointer, I am using &(sDatalog.wptr)

    if((*(uint32_t *)0xffe4) == 0xFFFFFFFF){
        sDatalog.wptr = (uint32_t *) DATALOG_MEMORY_START;
        FlashProgram(&(sDatalog.wptr), 0xffe4, sizeof(sDatalog.wptr));

    }else{
        sDatalog.wptr = (*(uint32_t *)0xffe4) ;
        // FlashProgram(&(sDatalog.wptr), 0xffe4, sizeof(sDatalog.wptr));

    }

    // start the microcontroller clock
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // just do it 10 times
     for(j = 0; j <=10; j++){

         sDatalog.buffer[0] = 'c';
         sDatalog.buffer[1] = 'o';
         sDatalog.buffer[2] = 'f';
         sDatalog.buffer[3] = 'f';
         sDatalog.buffer[4] = 'e';
         sDatalog.buffer[5] = 'e';
         sDatalog.buffer[6] = ' ';
         sDatalog.buffer[7] = 'i';
         sDatalog.buffer[8] = 's';
         sDatalog.buffer[9] = ' ';
         sDatalog.buffer[10] = 'g';
         sDatalog.buffer[11] = 'o';
         sDatalog.buffer[12] = 'o';
         sDatalog.buffer[13] = 'd';
         sDatalog.buffer[14] = '!';
         sDatalog.buffer[15] = '!';


         sDatalog.idx = 16;
         datalog_write_buffer();

     }

    while(1)
    {

    }
}

