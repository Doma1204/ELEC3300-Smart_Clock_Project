#ifndef EPOCHCONVERTER_h_
#define EPOCHCONVERTER_h_

#include<stdio.h>
#include<stdint.h>
#include "stm32f4xx.h"

typedef uint32_t epochTime;
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
uint32_t  now(void);           // the current epoch time
uint8_t   second(void);        // the second 
uint8_t   minute(void);        // the minute 
uint8_t   hour(void);          // the hour 
uint8_t   day(void);           // the day 
char      weekday(void);       // the weekday (Sunday is day 1) 
uint8_t   month(void);         // the month   (Jan is month 1)
uint16_t  year(void);          // the year

/* 12 hour format functions */
uint8_t hourFormat12(void);    // the hour in 12 hour format
uint8_t isAM(void);            // returns true if time is AM
uint8_t isPM(void);            // returns true if time is PM

void setTime(uint32_t epochTime);

#endif /* EPOCHCONVERTER_h_ */
