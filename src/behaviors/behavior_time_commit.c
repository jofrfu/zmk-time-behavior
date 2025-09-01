#define DT_DRV_COMPAT zmk_behavior_time_commit

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
#include <zephyr/settings/settings.h>
#include <zmk/behavior.h>
#include <stdlib.h>
#include "behavior_time.h"

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

// RTC
#define RTC_NODE DT_NODELABEL(rtc0)
static const struct device *rtc_dev = DEVICE_DT_GET(RTC_NODE);

// Clear Input
static void clear_input(void) {
    time_input_len = 0;
    time_input_buffer[0] = '\0';
    printk("RTC Input cleared\n");
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

    struct rtc_time rtc = {
        .tm_sec   = 0,
        .tm_min   = minute,
        .tm_hour  = hour,
        .tm_mday  = day,
        .tm_mon   = month,
        .tm_year  = year,
        .tm_wday  = -1,
        .tm_yday  = -1,
        .tm_isdst = -1,
        .tm_nsec  = 0
    };

    rtc_set_time(rtc_dev, &rtc);

    printk("RTC set to: %04d-%02d-%02d %02d:%02d\n",
           year, month, day, hour, minute);

    clear_input();
    return 0;
}


// ---------- Registrieren ----------
static const struct behavior_driver_api behavior_time_commit_api = {
    .binding_pressed = behavior_time_commit,
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_time_commit_api);


#endif