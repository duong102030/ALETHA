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

    // LOG_INF("== PLACE CALIBRATION WEIGHT (e.g. 196g) ON THE LOADCELL ==");
    // k_sleep(K_SECONDS(5)); // Đợi đặt vật chuẩn

    // sensors_calibration(196.0f);

    LOG_INF("== Calibration done. Now measuring weight ==");
    k_sleep(K_MSEC(500));

    while (1) {
        // int32_t value = sensors_read_load_cell();
        // LOG_INF("Loadcell digital value (zeroed): %d g\n", value);
        float weight = sensors_read_weight();
        LOG_INF("Loadcell measured: %.2f gram", weight);
        k_msleep(1000);
    }

    return 0;
}