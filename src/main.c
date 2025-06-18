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
    return 0;
}