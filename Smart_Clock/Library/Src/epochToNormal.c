// C program to demonstrate 
// example of time() function. 
#include <stdio.h> 
#include <time.h> 
#include "epochToNormal.h"

static tm t;
uint8_t TimeZone = 0;
static epochTime timeSet = 0;
static uint32_t tickSet = 0;
#define LEAP_YEAR(Y) ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0


void convertTime(epochTime input_time, uint8_t timezone){
    uint32_t time = input_time + timezone*3600;
    int8_t year;
    uint8_t month, monthLength;
    unsigned long days;

    t.Second = time % 60;
    time /= 60; // now it is minutes
    t.Minute = time % 60;
    time /= 60; // now it is hours
    t.Hour = time % 24;
    time /= 24; // now it is days
    t.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1 

    year = 0;  
    days = 0;
    while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
        year++;
    }
    t.Year = year+1970; // year is offset from 1970 
    
    days -= LEAP_YEAR(year) ? 366 : 365;
    time  -= days; // now it is days in this year, starting at 0
    
    days=0;
    month=0;
    monthLength=0;
    for (month=0; month<12; month++) {
        if (month==1) { // february
        if (LEAP_YEAR(year)) {
            monthLength=29;
        } else {
            monthLength=28;
        }
        } else {
        monthLength = monthDays[month];
        }
        
        if (time >= monthLength) {
        time -= monthLength;
        } else {
            break;
        }
    }
    t.Month = month + 1;  // jan is month 1  
    t.Day = time + 1;     // day of month
};

/* ========================================================================= */
uint32_t now(void) {
  return timeSet + (HAL_GetTick() - tickSet) / 1000;
}

uint8_t second() {  // the second now
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  return t.Second;
}

uint8_t minute() { // the minute now
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  return t.Minute;  
}

uint8_t hour() { // the hour now 
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  return t.Hour;
}

uint8_t day() { // the day now
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  return t.Day;
}

char weekday() { // Sunday is day 1
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  return t.Wday;
}

uint8_t month() {  // the month now
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  return t.Month;
}

uint16_t year() { // the year for the given time
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  return t.Year;
}

uint8_t hourFormat12() { // the hour now in 12 hour format
  epochTime now = timeSet + (HAL_GetTick() - tickSet) / 1000;
  convertTime(now, TimeZone);
  if( t.Hour == 0 )
    return 12; // 12 midnight
  else if( t.Hour  > 12)
    return t.Hour - 12 ;
  else
    return t.Hour ;
}

uint8_t isAM() { // returns true if time now is AM
  return !isPM(); 
}

uint8_t isPM() { // returns true if PM
  return (hour() >= 12); 
}

void setTime(uint32_t epochTime) {
  timeSet = epochTime;
  tickSet = HAL_GetTick();
}
