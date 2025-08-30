#pragma once

#include <zephyr/kernel.h>

struct rtc_state {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int64_t uptime_ref;
};

struct rtc_state rtc = {0};

#define MAX_TIME_DIGITS 12 // YYYYMMDDHHMM

char time_input_buffer[MAX_TIME_DIGITS + 1];
int time_input_len = 0;

void rtc_get_time(struct rtc_state *out);