#define DT_DRV_COMPAT zmk_behavior_time_commit

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
//#include <zephyr/settings/settings.h>
#include <zmk/behavior.h>
#include <stdlib.h>
#include "behavior_time.h"

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

// Clear Input
static void clear_input(void) {
    time_input_len = 0;
    time_input_buffer[0] = '\0';
    printk("RTC Input cleared\n");
}

// Save time persistently
/*static int save_time_values(void) {
    int err;

    err = settings_save_one("rtc/state", &rtc, sizeof(rtc));
    if (err) {
        printk("Failed to save time information (err %d)\n", err);
        return err;
    }

    printk("Saved RTC time information.\n");
    return 0;
}*/

// Load time after deep sleep
/*static int settings_load_cb(const char *key, size_t len,
                            settings_read_cb read_cb, void *cb_arg)
{
    if (strcmp(key, "state") == 0) {
        if (len == sizeof(rtc)) {
            read_cb(cb_arg, &rtc, sizeof(rtc));
        }
        return 0;
    }
    return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(rtc, "rtc", NULL, settings_load_cb, NULL, NULL);*/

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
    rtc.uptime_ref = k_uptime_get();//rtc_ticks_to_ms(rtc_get_ticks());

    printk("RTC set to: %04d-%02d-%02d %02d:%02d\n",
           year, month, day, hour, minute);

    clear_input();
    //save_time_values();
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