// C program to demonstrate 
// example of time() function. 
#include <stdio.h> 
#include <time.h> 
#include "epochConverter.h"

static tm t;
static epochTime now;
#define LEAP_YEAR(Y) ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
//char DayOfWeek[][255] = {"Sunday", "Monday", "Tuesday", "Wesnesday", "Thusdway", "Friday", "Saturday"};


void convertTime(epochTime input_time, uint8_t TimeZone){
    uint32_t time = input_time + TimeZone*3600;
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
int second() {  // the second now
  convertTime(now, TimeZone);
  return t.Second;
}

int minute() { // the minute now
  convertTime(now, TimeZone);
  return t.Minute;  
}

int hour() { // the hour now 
  convertTime(now, TimeZone);
  return t.Hour;
}

int day() { // the day now
  convertTime(now, TimeZone);
  return t.Day;
}

char weekday() { // Sunday is day 1
  convertTime(now, TimeZone);
  return t.Wday;
}
   
int month() {  // the month now
  convertTime(now, TimeZone);
  return t.Month;
}

int year() { // the year for the given time
  convertTime(now, TimeZone);
  return t.Year;
}

int hourFormat12() { // the hour now in 12 hour format
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

int main () 
{ 
    //time_t seconds; 
      
    //seconds = time(NULL); 
    //printf("Seconds since January 1, 1970 = %ld\n", seconds); 

    now = 1574356569;
    char wday[] = "";
    uint8_t Second = second(); 
    uint8_t Minute = minute(); 
    uint8_t Hour = hour(); 
    uint8_t Wday = weekday();  // day of week, sunday is day 1
    uint8_t Day = day();
    uint8_t Month = month(); 
    uint16_t Year = year();
    printf("the time is %dy %dm %dd %d %d:%d:%d\n", Year, Month, Day, Wday, Hour, Minute, Second);

    return 0;
} 
