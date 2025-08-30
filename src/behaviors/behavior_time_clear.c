#define DT_DRV_COMPAT zmk_behavior_time_clear

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
#include <zmk/behavior.h>
#include <stdlib.h>
#include <zmk/rtc_time.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)


// Clear Input
static void clear_input(void) {
    time_input_len = 0;
    time_input_buffer[0] = '\0';
    printk("RTC Input cleared\n");
}

// ---------- Behavior: Clear ----------
static int behavior_time_clear(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    clear_input();
    return 0;
}


// ---------- Registrieren ----------
static const struct behavior_driver_api behavior_time_clear_api = {
    .binding_pressed = behavior_time_clear,
};



BEHAVIOR_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &behavior_time_clear_api);


#endif