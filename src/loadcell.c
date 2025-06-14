#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "loadcell.h"
#include "../drivers/sensor/nau7802/nau7802.h"

LOG_MODULE_REGISTER(loadcell, CONFIG_LOG_DEFAULT_LEVEL);

static const struct device *nau7802_dev;
static float offset_grams = 0.0f;
static float scale_factor = 0.1f; // Hệ số ban đầu, sẽ được hiệu chỉnh

int loadcell_init(const struct device *dev) {
    if (!device_is_ready(dev)) {
        LOG_ERR("NAU7802 device not ready");
        return -1;
    }
    nau7802_dev = dev;
    LOG_INF("Loadcell initializing...");

    // Hiệu chỉnh zero (không có lực)
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

int loadcell_calibrate_span(float known_weight) {
    struct sensor_value val;
    float raw_value = 0.0f;
    for (int i = 0; i < 10; i++) {
        sensor_sample_fetch(nau7802_dev);
        sensor_channel_get(nau7802_dev, SENSOR_CHAN_FORCE, &val);
        raw_value += sensor_value_to_double(&val);
        k_sleep(K_MSEC(100));
    }
    raw_value /= 10.0f;

    // Tính hệ số chuyển đổi dựa trên trọng lượng tham chiếu
    scale_factor = (raw_value - sensor_value_to_double(&val) * offset_grams) / known_weight;
    LOG_INF("Span calibrated, scale factor: %.6f", scale_factor);
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

    float raw_grams = (sensor_value_to_double(&val) - offset_grams) / scale_factor;
    if (raw_grams < 0) {
        raw_grams = 0.0f;
    }

    *grams = raw_grams;
    LOG_INF("Weight: %.2f g", raw_grams);
    return 0;
}