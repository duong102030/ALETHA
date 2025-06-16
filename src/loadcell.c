#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "loadcell.h"
#include "../drivers/sensor/nau7802/nau7802.h"

LOG_MODULE_REGISTER(loadcell, CONFIG_LOG_DEFAULT_LEVEL);

static const struct device *nau7802 = DEVICE_DT_GET_ONE(nuvoton_nau7802);

static const float invalid_val = -255.0f;

sensors_t sensors = {0};

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
    LOG_INF("Sample: %ld, Weight: %.2f", ((struct nau7802_data *)nau7802->data)->sample, (float)weight_dbl);

	return (float)weight_dbl;
}


static int nau7802_tare(const struct device *dev, uint8_t readings)
{
    int64_t sum = 0;
    int ret;

    if (readings == 0) {
        readings = 1;
    }

    /* Take multiple samples and sum them */
    for (int i = 0; i < readings; i++) {
        ret = sensor_sample_fetch(dev);
        if (ret != 0) {
            LOG_ERR("Failed to fetch sample %d: %d", i, ret);
            return ret;
        }
        struct sensor_value sample;
        ret = sensor_channel_get(dev, SENSOR_CHAN_FORCE, &sample);
        if (ret != 0) {
            LOG_ERR("Failed to get sample %d: %d", i, ret);
            return ret;
        }
        sum += sensor_value_to_double(&sample);
    }

    /* Calculate average */
    float32_t avg = (float32_t)(sum / readings);

    /* Prepare sensor_value for offset */
    struct sensor_value offset;
    sensor_value_from_float(&offset, avg);

    /* Set the offset using sensor API */
    ret = sensor_attr_set(dev, SENSOR_CHAN_FORCE, SENSOR_ATTR_OFFSET, &offset);
    if (ret != 0) {
        LOG_ERR("Failed to set offset: %d", ret);
        return ret;
    }

    LOG_DBG("Tare completed, offset set to %f", (double)avg);
    return 0;
}


//double check
int initialize_load_cell(void)
{
    if (!nau7802) {
        LOG_ERR("NAU7802 not found");
        return -ENODEV;
    }
    if (!device_is_ready(nau7802)) {
        LOG_ERR("NAU7802 device %s is not ready", nau7802->name);
        return -EIO;
    }

    k_msleep(500); /* wait for startup stabilization */

    LOG_INF("Device is %p, name is %s", nau7802, nau7802->name);
    LOG_INF("NAU7802: Calculating tare...");

    /* Perform tare to set offset */
    int ret = nau7802_tare(nau7802, 10);
    if (ret != 0) {
        LOG_ERR("Failed to calculate tare: %d", ret);
        return ret;
    }

    LOG_INF("NAU7802 successfully initialized");
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