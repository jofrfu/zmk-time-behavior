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

void rtc_get_time(struct rtc_state *out);