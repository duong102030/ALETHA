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

	LOG_INF("NAU7802 succesfully initialized");
	return 0;
}
