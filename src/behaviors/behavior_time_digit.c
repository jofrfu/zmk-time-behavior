#define DT_DRV_COMPAT zmk_behavior_time_digit

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
#include <zmk/behavior.h>
#include <stdlib.h>
#include "behavior_time.h"

char time_input_buffer[MAX_TIME_DIGITS + 1];
int time_input_len = 0;

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

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

// ---------- Registrieren ----------
static const struct behavior_driver_api behavior_time_digit_api = {
    .binding_pressed = behavior_time_digit,
};

BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_time_digit_api);


#endif