#pragma once

#include <zmk_behavior_time/rtc_time.h>

//uint32_t rtc_get_ticks(void);
//uint64_t rtc_ticks_to_ms(uint32_t ticks);


extern struct rtc_state rtc;

#define MAX_TIME_DIGITS 12 // YYYYMMDDHHMM

extern char time_input_buffer[MAX_TIME_DIGITS + 1];
extern int time_input_len;