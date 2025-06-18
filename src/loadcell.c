#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "loadcell.h"
#include "../drivers/sensor/nau7802/nau7802.h"

LOG_MODULE_REGISTER(loadcell, CONFIG_LOG_DEFAULT_LEVEL);

static const struct device *const nau7802 = DEVICE_DT_GET_ONE(nuvoton_nau7802);

static int32_t zero_offset = 0;

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

	k_msleep(500); // wait for startup stabilization

	LOG_DBG("Device is %p, name is %s", nau7802, nau7802->name);

	LOG_INF("NAU7802: Calculating tare...");

    int ret = nau7802_tare(nau7802, 10);
    if (ret == -EIO || ret == -ENODEV) {
    LOG_ERR("NAU7802 tare failed: %d", ret);
    return ret;
    }
	LOG_INF("NAU7802 succesfully initialized");
	return 0;
}

int nau7802_tare(const struct device *dev, uint8_t readings)
{
    int64_t avg = 0;
    struct sensor_value val;

    if (readings == 0) {
        readings = 1;
    }

    for (int i = 0; i < readings; i++) {
        //Fetch a sample from the sensor
        int ret = sensor_sample_fetch(dev);
        if (ret) {
            LOG_ERR("Failed to fetch sensor sample at %d", i);
            return ret;
        }

        //Get a reading from a sensor device
        ret = sensor_channel_get(dev, SENSOR_CHAN_FORCE, &val);
        if (ret) {
            LOG_ERR("Failed to get sensor value at %d", i);
            return ret;
        }

        avg += val.val1;
        k_msleep(10); // sleep to stabilize ADC
    }

    //Calculate the average value: sum of values ​​divided by number of readings
    zero_offset = avg / readings;
    LOG_INF("Tare complete, zero offset: %d", zero_offset);

    return zero_offset;
}

int32_t nau7802_measure(void)
{
    struct sensor_value val;

    //Fetch a sample from the sensor
    int ret = sensor_sample_fetch(nau7802);
    if (ret) {
        LOG_ERR("Failed to fetch sensor sample");
        return 0;
    }

    //Get a reading from a sensor device
    ret = sensor_channel_get(nau7802, SENSOR_CHAN_FORCE, &val);
    if (ret) {
        LOG_ERR("Failed to get sensor value");
        return 0;
    }
    return val.val1;
}

int32_t sensors_read_load_cell(void)
{
	int32_t loadcell_value = nau7802_measure();
    // If the reading is negative (due to noise/error), consider it as 0:
    if (loadcell_value < 0.0f) {
        loadcell_value = 0.0f;
    }
    return loadcell_value;
}