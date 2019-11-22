#ifndef EPOCHCONVERTER_h_
#define EPOCHCONVERTER_h_
#include<stdio.h>
#include<stdint.h>

typedef uint32_t epochTime;
uint8_t TimeZone = 8;
char Month[][255] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
typedef struct TimeElements{
    uint8_t Second; 
    uint8_t Minute; 
    uint8_t Hour; 
    uint8_t Wday;   // day of week, sunday is day 1
    uint8_t Day;
    uint8_t Month; 
    uint16_t Year;   // offset from 1970; 
}tm;

void convertTime(epochTime input_time, uint8_t TimeZone);         //convert epoch time to human readable time and save in TimeElements

/* Basic time elements functions */
int     second();          // the second 
int     minute();          // the minute 
int     hour();            // the hour 
int     day();             // the day 
char    weekday();         // the weekday (Sunday is day 1) 
int     month();           // the month  (Jan is month 1)


/* 12 hour format functions */
int     hourFormat12();    // the hour in 12 hour format
uint8_t isAM();            // returns true if time is AM
uint8_t isPM();            // returns true if time is PM

#endif /* EPOCHCONVERTER_h_ */