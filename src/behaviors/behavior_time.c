#define DT_DRV_COMPAT zmk_behavior_time

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
#include <zmk/behavior.h>
#include <stdlib.h>
#include "rtc_time.h"

#define MAX_TIME_DIGITS 12 // YYYYMMDDHHMM

static struct rtc_state rtc = {0};

static char time_input_buffer[MAX_TIME_DIGITS + 1];
static int time_input_len = 0;

// ---------- Helper  Function ----------
static bool is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int days_in_month(int year, int month) {
    switch (month) {
    case 1: return 31;
    case 2: return is_leap_year(year) ? 29 : 28;
    case 3: return 31;
    case 4: return 30;
    case 5: return 31;
    case 6: return 30;
    case 7: return 31;
    case 8: return 31;
    case 9: return 30;
    case 10: return 31;
    case 11: return 30;
    case 12: return 31;
    default: return 30;
    }
}

// Append Digit
static void append_digit(int digit) {
    if (time_input_len < MAX_TIME_DIGITS) {
        time_input_buffer[time_input_len++] = '0' + digit;
        time_input_buffer[time_input_len] = '\0';
        printk("RTC Input: %s\n", time_input_buffer);
    } else {
        printk("RTC Input buffer full!\n");
    }
}

// Clear Input
static void clear_input(void) {
    time_input_len = 0;
    time_input_buffer[0] = '\0';
    printk("RTC Input cleared\n");
}

// ---------- Behavior: Digit ----------
static int behavior_time_digit(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    int digit = binding->param1;
    if (digit >= 0 && digit <= 9) {
        append_digit(digit);
    }
    return 0;
}

// ---------- Behavior: Clear ----------
static int behavior_time_clear(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    clear_input();
    return 0;
}

// ---------- Behavior: Commit ----------
static int behavior_time_commit(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
    if (time_input_len < MAX_TIME_DIGITS) {
        printk("RTC Commit failed: not enough digits (%d/%d)\n", time_input_len, MAX_TIME_DIGITS);
        return -EINVAL;
    }

    char buf[5];
    memcpy(buf, &time_input_buffer[0], 4); buf[4] = '\0';
    int year = atoi(buf);

    memcpy(buf, &time_input_buffer[4], 2); buf[2] = '\0';
    int month = atoi(buf);

    memcpy(buf, &time_input_buffer[6], 2); buf[2] = '\0';
    int day = atoi(buf);

    memcpy(buf, &time_input_buffer[8], 2); buf[2] = '\0';
    int hour = atoi(buf);

    memcpy(buf, &time_input_buffer[10], 2); buf[2] = '\0';
    int minute = atoi(buf);

    rtc.year = year;
    rtc.month = month;
    rtc.day = day;
    rtc.hour = hour;
    rtc.minute = minute;
    rtc.second = 0;
    rtc.uptime_ref = k_uptime_get();

    printk("RTC set to: %04d-%02d-%02d %02d:%02d\n",
           year, month, day, hour, minute);

    clear_input();
    return 0;
}

// ---------- Zeit berechnen ----------
void rtc_get_time(struct rtc_state *out) {
    int64_t diff_ms = k_uptime_get() - rtc.uptime_ref;
    int add_seconds = diff_ms / 1000;

    *out = rtc;

    out->second += add_seconds;

    while (out->second >= 60) {
        out->second -= 60;
        out->minute++;
    }
    while (out->minute >= 60) {
        out->minute -= 60;
        out->hour++;
    }
    while (out->hour >= 24) {
        out->hour -= 24;
        out->day++;
    }

    while (1) {
        int dim = days_in_month(out->year, out->month);
        if (out->day > dim) {
            out->day -= dim;
            out->month++;
            if (out->month > 12) {
                out->month = 1;
                out->year++;
            }
        } else break;
    }
}

// ---------- Registrieren ----------
static const struct behavior_driver_api behavior_time_digit_api = {
    .binding_pressed = behavior_time_digit,
};

static const struct behavior_driver_api behavior_time_clear_api = {
    .binding_pressed = behavior_time_clear,
};

static const struct behavior_driver_api behavior_time_commit_api = {
    .binding_pressed = behavior_time_commit,
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_time_digit_api);

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_time_clear_api);

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_time_commit_api);
