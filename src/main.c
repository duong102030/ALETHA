#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>

#include "loadcell.h"
/* this is for getting the SENSOR_CHAN_FORCE*/
// #include "../drivers/sensor/nau7802/nau7802.h"

LOG_MODULE_REGISTER(main_logging);

int main(void)
{
    // const struct device *nau7802 = DEVICE_DT_GET_ONE(nuvoton_nau7802);

    // if (!device_is_ready(nau7802)) {
    //     LOG_ERR("NAU7802 is not ready");
    //     return;
    // }

    // // LOG_INF("NAU7802 detected!");

    // if (loadcell_init(nau7802) < 0) {
    //     return -1;
    // }

    // LOG_INF("NAU7802 detected!");

    // while (1) {
    //     float grams;
    //     if (loadcell_read_grams(&grams) == 0) {
    //         k_sleep(K_SECONDS(1)); // Đọc mỗi giây
    //     }
    // }
    
    int ret;

    /* Initialize the load cell */
    ret = initialize_load_cell();
    if (ret != 0) {
        LOG_ERR("Failed to initialize load cell: %d", ret);
        return;
    }

    /* Main loop to read load cell periodically */
    while (1) {
        sensors_read_load_cell();
        LOG_INF("Load cell weight: %.2f", sensors.weight);
        k_msleep(1000); /* Read every 1 second */
    }
    return 0;
}