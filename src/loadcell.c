#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "loadcell.h"
#include "../drivers/sensor/nau7802/nau7802.h"

LOG_MODULE_REGISTER(loadcell, CONFIG_LOG_DEFAULT_LEVEL);

// static const struct device *nau7802_dev;
// static float offset_grams = 0.0f;
// static float scale_factor = 0.1f; // Hệ số ban đầu, sẽ được hiệu chỉnh

// int loadcell_init(const struct device *dev) {
//     if (!device_is_ready(dev)) {
//         LOG_ERR("NAU7802 device not ready");
//         return -1;
//     }
//     nau7802_dev = dev;
//     LOG_INF("Loadcell initializing...");

//     // Hiệu chỉnh zero (không có lực)
//     float temp_grams;
//     for (int i = 0; i < 10; i++) {
//         loadcell_read_grams(&temp_grams);
//         offset_grams += temp_grams;
//         k_sleep(K_MSEC(100));
//     }
//     offset_grams /= 10.0f;
//     LOG_INF("Offset calibrated: %.2f g", offset_grams);
//     return 0;
// }

// int loadcell_calibrate_span(float known_weight) {
//     struct sensor_value val;
//     float raw_value = 0.0f;
//     for (int i = 0; i < 10; i++) {
//         sensor_sample_fetch(nau7802_dev);
//         sensor_channel_get(nau7802_dev, SENSOR_CHAN_FORCE, &val);
//         raw_value += sensor_value_to_double(&val);
//         k_sleep(K_MSEC(100));
//     }
//     raw_value /= 10.0f;

//     // Tính hệ số chuyển đổi dựa trên trọng lượng tham chiếu
//     scale_factor = (raw_value - sensor_value_to_double(&val) * offset_grams) / known_weight;
//     LOG_INF("Span calibrated, scale factor: %.6f", scale_factor);
//     return 0;
// }

// int loadcell_read_grams(float *grams) {
//     struct sensor_value val;
//     int ret = sensor_sample_fetch(nau7802_dev);
//     if (ret < 0) {
//         LOG_ERR("Failed to fetch sample: %d", ret);
//         return ret;
//     }

//     ret = sensor_channel_get(nau7802_dev, SENSOR_CHAN_FORCE, &val);
//     if (ret < 0) {
//         LOG_ERR("Failed to get force value: %d", ret);
//         return ret;
//     }

//     float raw_grams = (sensor_value_to_double(&val) - offset_grams) / scale_factor;
//     if (raw_grams < 0) {
//         raw_grams = 0.0f;
//     }

//     *grams = raw_grams;
//     LOG_INF("Weight: %.2f g", raw_grams);
//     return 0;
// }

static const struct device *nau7802 = DEVICE_DT_GET_ONE(nuvoton_nau7802);

static const float invalid_val = -255.0f;

static sensors_t sensors = {0};

static float nau7802_measure(void)
{
	static struct sensor_value weight;
	int ret;

	ret = sensor_sample_fetch(nau7802);
	if (ret != 0) {
		LOG_ERR("Cannot take measurement: %d", ret);
		return invalid_val;
	}

	sensor_channel_get(nau7802, SENSOR_CHAN_FORCE, &weight);
	double weight_dbl = sensor_value_to_double(&weight);

	return (float)weight_dbl;
}


static int nau7802_tare(const struct device *dev, uint8_t readings)
{
    struct nau7802_data *data = dev->data;
    int64_t sum = 0;
    int ret;

    if (readings == 0) {
        readings = 1;
    }

    /* Take multiple samples and sum them */
    for (int i = 0; i < readings; i++) {
        ret = nau7802_sample_fetch(dev, SENSOR_CHAN_ALL);
        if (ret != 0) {
            LOG_ERR("Failed to fetch sample %d: %d", i, ret);
            return ret;
        }
        sum += data->sample;
    }

    /* Calculate average */
    float32_t avg = (float32_t)(sum / readings);

    /* Prepare sensor_value for offset */
    struct sensor_value offset;
    memcpy(&offset.val1, &avg, sizeof(float32_t));
    offset.val2 = 0;

    /* Set the offset */
    ret = nau7802_setOffset(dev, &offset);
    if (ret != 0) {
        LOG_ERR("Failed to set offset: %d", ret);
        return ret;
    }

    LOG_DBG("Tare completed, offset set to %f", (float)avg);
    return 0;
}

//double check
static int initialize_load_cell(void)
{
	if (!nau7802) {
		LOG_ERR("NAU7802 not found");
		return -ENODEV;
	}
	if (!device_is_ready(nau7802)) {
		LOG_ERR("NAU7802 device %s is not ready", nau7802->name);
		return -EIO;
	}

	k_msleep(500); // wait for startup stabilization

	LOG_INF("Device is %p, name is %s", nau7802, nau7802->name);
	LOG_INF("NAU7802: Calculating tare...");
	/* Perform tare to set offset */
    int ret = nau7802_tare(nau7802, 10);
    if (ret != 0) {
        LOG_ERR("Failed to calculate tare: %d", ret);
        return ret;
    }

	LOG_INF("NAU7802 succesfully initialized");
	return 0;
}

void sensors_read_load_cell(void)
{
	float loadcell_value = nau7802_measure();
    // If the reading is negative (due to noise/error), consider it as 0:
    if (loadcell_value < 0.0f) {
        loadcell_value = 0.0f;
    }
    sensors.weight = loadcell_value;
}