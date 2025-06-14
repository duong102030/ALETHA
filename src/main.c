#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
/* this is for getting the SENSOR_CHAN_FORCE*/
#include "../drivers/sensor/nau7802/nau7802.h"

LOG_MODULE_REGISTER(main_logging);

int main(void)
{
    const struct device *nau7802 = DEVICE_DT_GET_ONE(nuvoton_nau7802);

    if (!device_is_ready(nau7802)) {
        LOG_ERR("NAU7802 is not ready");
        return;
    }

    LOG_INF("NAU7802 detected!");
    
    return 0;
}