#define DT_DRV_COMPAT zmk_behavior_time_digit

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
#include <zmk/behavior.h>
#include <stdlib.h>
#include <zmk_behavior_time/rtc_time.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

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

// ---------- Behavior: Digit ----------
static int behavior_time_digit(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    int digit = binding->param1;
    if (digit >= 0 && digit <= 9) {
        append_digit(digit);
    }
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

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_time_digit_api);


#endif