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
    int ret;

    /* Initialize the load cell */
    ret = initialize_load_cell();
    if (ret != 0) {
        LOG_ERR("Failed to initialize load cell: %d", ret);
        return ret;
    }

    /* Main loop to read load cell periodically */
    while (1) {
        sensors_read_load_cell();
        LOG_INF("Load cell weight: %.2f g", sensors.weight);
        k_msleep(1000); /* Read every 1 second */
    }
    return 0;
}