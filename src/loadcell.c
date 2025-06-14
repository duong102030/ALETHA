#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "loadcell.h"
#include "../drivers/sensor/nau7802/nau7802.h"

LOG_MODULE_REGISTER(loadcell, CONFIG_LOG_DEFAULT_LEVEL);

static const struct device *nau7802_dev;
static float offset_grams = 0.0f;

int loadcell_init(const struct device *dev) {
    if (!device_is_ready(dev)) {
        LOG_ERR("NAU7802 device not ready");
        return -1;
    }
    nau7802_dev = dev;
    LOG_INF("Loadcell initializing...");

    // Lấy offset khi không có lực
    float temp_grams;
    for (int i = 0; i < 10; i++) {
        loadcell_read_grams(&temp_grams);
        offset_grams += temp_grams;
        k_sleep(K_MSEC(100));
    }
    offset_grams /= 10.0f;
    LOG_INF("Offset calibrated: %.2f g", offset_grams);
    return 0;
}

int loadcell_read_grams(float *grams) {
    struct sensor_value val;
    int ret = sensor_sample_fetch(nau7802_dev);
    if (ret < 0) {
        LOG_ERR("Failed to fetch sample: %d", ret);
        return ret;
    }

    ret = sensor_channel_get(nau7802_dev, SENSOR_CHAN_FORCE, &val);
    if (ret < 0) {
        LOG_ERR("Failed to get force value: %d", ret);
        return ret;
    }

    // Chuyển đổi giá trị sensor sang gram (giả sử 1 đơn vị force = 0.1g, cần hiệu chỉnh thực tế)
    float force_grams = sensor_value_to_double(&val) * 0.1f;
    if (force_grams < 0) {
        force_grams = 0.0f;
    }

    *grams = force_grams;
    LOG_INF("Weight: %.2f g", force_grams);
    return 0;
}