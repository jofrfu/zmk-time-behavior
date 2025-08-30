#pragma once

#include <zephyr/kernel.h>

struct rtc_state {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int dow; // Day of week 0-6
    int64_t uptime_ref;
};

extern struct rtc_state rtc;

#define MAX_TIME_DIGITS 12 // YYYYMMDDHHMM

extern char time_input_buffer[MAX_TIME_DIGITS + 1];
extern int time_input_len;

void rtc_get_time(struct rtc_state *out);