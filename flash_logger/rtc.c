/*
 * rtc.c
 *
 *  Created on: Jul 6, 2017
 *      Author: Onofre
 */


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "rtc.h"


// Global variables

//Local Functions:
static void I2C1_Init(void){
    //
    // The I2C1 peripheral must be enabled before use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    //
    // For this example I2C1 is used with PortB[3:2].  The actual port and
    // pins used may be different on your part, consult the data sheet for
    // more information.  GPIO port A needs to be enabled so these pins can
    // be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for I2C1 functions on port A6 and A7.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    //
    // Select the I2C function for these pins.  This function will also
    // configure the GPIO pins pins for I2C operation, setting them to
    // open-drain operation with weak pull-ups.  Consult the data sheet
    // to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    //
    // Enable and initialize the I2C1 master module.  Use the system clock for
    // the I2C1 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    //
    #if defined(TARGET_IS_TM4C129_RA0) ||                                         \
        defined(TARGET_IS_TM4C129_RA1) ||                                         \
        defined(TARGET_IS_TM4C129_RA2)
        I2CMasterInitExpClk(I2C1_BASE, ui32SysClock, false);
    #else
        I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);
    #endif

}

static void Conversion_Decimal_to_Hexa(uint8_t in_dec, uint8_t *out_hexa){

    *out_hexa = 0x0;

    if(in_dec<=9){
        *out_hexa = in_dec + 0x0;
    }else{
//         in_dec = 25
//         in_dec / 10 = 2;
//         out_hexa = in_dec = 2
//         out_hexa << 4  = 20 ;
//         in_dec % 10 = 5
//         out_hexa + in_dec = 0x25;

        *out_hexa = ((((in_dec)/10)) <<4) + (((in_dec)%10)+0x0);

    }
}



//Global Functions:
void RTC_Init(void){
    I2C1_Init();        // 1.   Start the I2C bus.
    RTC_Enable();       // 2.   Enable RTC
    RTC_SQW_Disable();  // 3.   Disable Square Wave
}


void RTC_Enable(void){

    I2CMasterSlaveAddrSet(I2C1_BASE, RTC_ADDRESS, false);  // 2. Send the DS1307 address and select write operation

    I2CMasterDataPut(I2C1_BASE, 0x0);   // 3.    Send the seconds Register. 0x00.

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

    I2CMasterDataPut(I2C1_BASE, 0x0); // 4. Write 0x00. This Enable the RTC Module. Bit 7 = 0 -> RTC ON

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);  //5. Stop communication after this byte.

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

}

 void RTC_Disable(void){

    I2CMasterSlaveAddrSet(I2C1_BASE, RTC_ADDRESS, false);  // 1. Send the DS1307 address and select write operation

    I2CMasterDataPut(I2C1_BASE, 0x0);   // 2.    Send the seconds Register. 0x00.

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

    I2CMasterDataPut(I2C1_BASE, 0x80); // 3. Write 0x00. This turns OFF the RTC Module. Bit 7 = 1 -> RTC OFF

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);  //4. Stop communication after this byte.

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

}

void RTC_SQW_Enable(void){

    I2CMasterSlaveAddrSet(I2C1_BASE, RTC_ADDRESS, false); // 2. Send the DS1307 address and select write operation

    I2CMasterDataPut(I2C1_BASE, 0x07);      // 3.    Send the Address of Control Register for sending the command.

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

    I2CMasterDataPut(I2C1_BASE, 0x10); // 4.  Send the Command to Enable the SQW Out @ 1Hz

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // 5. Stop the Communication.

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

}

void RTC_SQW_Disable(void){

    I2CMasterSlaveAddrSet(I2C1_BASE, RTC_ADDRESS, false); // 2. Send the DS1307 address and select write operation

    I2CMasterDataPut(I2C1_BASE, 0x07);      // 3.    Send the Address of Control Register for sending the command.

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

    I2CMasterDataPut(I2C1_BASE, 0x0); // 4.  Send the Command to Disable the SQW Out.
    // I2CMasterDataPut(I2C1_BASE, 0x10); // Command to Enable the SQW Out @ 1Hz

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // 5. Stop the Communication.

    while(I2CMasterBusy(I2C1_BASE)); // Wait until master module is done transferring.

}


void RTC_SetDateTime(rtc_t *rtc){

     I2CMasterSlaveAddrSet(I2C1_BASE, RTC_ADDRESS, false);      //0) set as writing mode

     I2CMasterDataPut(I2C1_BASE, 0x00);      //1) set the register address 0x0

     I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START); //2) there will be multiple bytes

     while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.

     //3) load seconds
         I2CMasterDataPut(I2C1_BASE, rtc->sec);

         I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

         while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.

     //4) load minutes
         I2CMasterDataPut(I2C1_BASE, rtc->min);

         I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

         while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.


     //5) load hours
         I2CMasterDataPut(I2C1_BASE, rtc->hour);

         I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

         while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.

     //6) load day of week
         I2CMasterDataPut(I2C1_BASE, rtc->weekDay);

         I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

         while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.

     //7) load date
         I2CMasterDataPut(I2C1_BASE, rtc->date);

         I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

         while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.

     //8) load month
         I2CMasterDataPut(I2C1_BASE, rtc->month);

         I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

         while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.

     //8) load year
         I2CMasterDataPut(I2C1_BASE, rtc->year);

         I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);  // Stop I2C communication after Setting the Date

         while(I2CMasterBusy(I2C1_BASE));         // Wait until master module is done transferring.

}


void RTC_GetDateTime(rtc_t *rtc){


    I2CMasterSlaveAddrSet(I2C1_BASE, RTC_ADDRESS, false);   // 1) send rtc address in write mode

    I2CMasterDataPut(I2C1_BASE, 0x0);            // 2) set register pointer address to 0x00. Seconds

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    while(I2CMasterBusy(I2C1_BASE));            // Wait until master module is done transferring.

    I2CMasterSlaveAddrSet(I2C1_BASE, RTC_ADDRESS, true);            // 3) send rtc address in reading mode

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START); // There will be multiple bytes

    while(I2CMasterBusy(I2C1_BASE));            // Wait until master module is done transferring.

    rtc->sec = I2CMasterDataGet(I2C1_BASE);     // 4) receive seconds

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

    while(I2CMasterBusy(I2C1_BASE));            // Wait until master module is done transferring.

    rtc->min = I2CMasterDataGet(I2C1_BASE);     // 5) receive minutes


    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

    while(I2CMasterBusy(I2C1_BASE));            // Wait until master module is done transferring.

    rtc->hour = I2CMasterDataGet(I2C1_BASE);    // 6) hour

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

    while(I2CMasterBusy(I2C1_BASE));            // Wait until master module is done transferring.

    rtc->weekDay = I2CMasterDataGet(I2C1_BASE); // 7) Receive week day

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

    while(I2CMasterBusy(I2C1_BASE));             // Wait until master module is done transferring.

    rtc->date = I2CMasterDataGet(I2C1_BASE);     // 8) Receive date

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);

    while(I2CMasterBusy(I2C1_BASE));             // Wait until master module is done transferring.

    rtc->month = I2CMasterDataGet(I2C1_BASE);    // 9) Receive month

    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    while(I2CMasterBusy(I2C1_BASE));             // Wait until master module is done transferring.

    rtc->year = I2CMasterDataGet(I2C1_BASE);     // 10) receive year

}



/*
 * Set time as 24H: HH MM SS and rtc address
 *
 * input range:
 *
 * hours from 0 to 23
 * minutes from 0 to 59
 * seconds from 0 to 59
 *
 * As the variables are 8bits the legal input range is from 0 to 255.
 * Values smaller than zero and greater than 255 will generate errors .
 *
 *
 */
uint8_t RTC_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, rtc_t *rtc){
    uint8_t error;

    if((hours>=0) && (hours<=23)){
        Conversion_Decimal_to_Hexa(hours  , & rtc->hour);
        error = 0;
    }else{
        error += 1;
    }

    if((minutes>=0) && (minutes<=59)){
        Conversion_Decimal_to_Hexa(minutes, & rtc->min);
        error = 0;
    }else{
        error += 2;
    }

    if((seconds>=0) && (seconds<=59)){
        Conversion_Decimal_to_Hexa(seconds, & rtc->sec);
    }else{
        error += 3;
    }
    return(error); //error

}


/*
 * set date as:
 *      weekday (Sun, Mon, Tue, Wed, Thu, Fri, Sat)
 *              ( 1    2    3    4    5    6    7 )
 *
 *      month  (Jan, Feb, Mar, Apr, May, Jun, Jul, Ago, Sep, Oct, Nov, Dec
 *             ( 1    2    3    4    5    6    7    8    9   10   11   12 )
 *
 *      date 0 - 31
 *
 *      year 00 ~ 99
 *
 *
 */

uint8_t RTC_set_date(weekDayAbb_t weekDay, uint8_t date, MonthAbb_t month, uint8_t year, rtc_t *rtc){
    uint8_t error;

    if((weekDay >= 1) && (weekDay <= 7)){

        Conversion_Decimal_to_Hexa(weekDay  , &rtc->weekDay);
          error = 0;
      }else{
          error += 1;
      }

      if((date >= 1) && (date <= 31)){
          Conversion_Decimal_to_Hexa(date  , &rtc->date);
          error = 0;
      }else{
          error += 2;
      }

      if((month >= 1) && (month <= 12)){
          Conversion_Decimal_to_Hexa(month, &rtc->month);
      }else{
          error += 3;
      }

      if((year >= 0) && (year <= 99)){
          Conversion_Decimal_to_Hexa(year, &rtc->year);
      }else{
          error += 4;
      }
      return(error); //error


}



void RTC_timeStamp(rtc_t *rtc, uint8_t *pt_TimeStamp ){

//    pt_TimeStamp[0] = rtc->year;
//    pt_TimeStamp[1] = rtc->month;
//    pt_TimeStamp[2] = rtc->date;
//    pt_TimeStamp[3] = rtc->hour;
//    pt_TimeStamp[4] = rtc->min;
//    pt_TimeStamp[5] = rtc->sec;
//    pt_TimeStamp[6] = 0;

    pt_TimeStamp[0] = rtc->year;
    pt_TimeStamp[1] = ',';
    pt_TimeStamp[2] = rtc->month;
    pt_TimeStamp[3] = ',';
    pt_TimeStamp[4] = rtc->date;
    pt_TimeStamp[5] = ',';
    pt_TimeStamp[6] = rtc->hour;
    pt_TimeStamp[7] = ',';
    pt_TimeStamp[8] = rtc->min;
    pt_TimeStamp[9] = ',';
    pt_TimeStamp[10] = rtc->sec;
    pt_TimeStamp[11] = ',';
    pt_TimeStamp[12] = 0;

}

