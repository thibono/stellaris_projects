/*
 * rtc.h
 *
 *  Created on: Jul 6, 2017
 *      Author: Onofre
 */

#ifndef RTC_H_
#define RTC_H_

//*****************************************************************************
//
// Set the address for slave module. This is a 7-bit address sent in the
// following format:
//                      [A6:A5:A4:A3:A2:A1:A0:RS]
//
// A zero in the "RS" position of the first byte means that the master
// transmits (sends) data to the selected slave, and a one in this position
// means that the master receives data from the slave.
//
//*****************************************************************************
#define RTC_ADDRESS 0x68

 typedef struct
 {
     uint8_t sec;
     uint8_t min;
     uint8_t hour;
     uint8_t weekDay;
     uint8_t date;
     uint8_t month;
     uint8_t year;
 }rtc_t;


// typedef enum {
//    Sunday = 1,
//    Monday,
//    Tuesday,
//    Wednesday,
//    Thursday,
//    Friday,
//    Saturday
//}weekDay_t;


typedef enum {
     Sun = 1,
     Mon,
     Tue,
     Wed,
     Thu,
     Fri,
     Sat
}weekDayAbb_t;

typedef enum {
     Jan = 1,
     Feb,
     Mar,
     Apr,
     May,
     Jun,
     Jul,
     Ago,
     Sep,
     Oct,
     Nov,
     Dec
}MonthAbb_t;


 void RTC_Init(void);
 void RTC_Enable(void);
 void RTC_Disable(void);
 void RTC_SQW_Enable(void);
 void RTC_SQW_Disable(void);
 void RTC_SetDateTime(rtc_t *rtc);
 void RTC_GetDateTime(rtc_t *rtc);
 uint8_t RTC_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds, rtc_t *rtc);
 uint8_t RTC_set_date(weekDayAbb_t weekDay, uint8_t date, MonthAbb_t month, uint8_t year, rtc_t *rtc);
 void RTC_timeStamp(rtc_t *rtc, uint8_t *pt_TimeStamp );

#endif /* RTC_H_ */
