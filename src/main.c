#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include "loadcell.h"


LOG_MODULE_REGISTER(main_logging);

int main(void)
{
    int ret = initialize_load_cell();
    if (ret) {
        LOG_ERR("Loadcell init failed: %d", ret);
        return;
    }

    while (1) {
        int32_t value = sensors_read_load_cell();
        LOG_INF("Loadcell digital value (zeroed): %d g\n", value);
        k_msleep(1000);
    }

    return 0;
}