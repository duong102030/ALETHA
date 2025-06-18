#ifndef LOADCELL_H
#define LOADCELL_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the NAU7802 load cell sensor.
 *
 * This function checks if the NAU7802 device is present and ready,
 * waits for sensor stabilization, and performs an initial tare
 * (zeroing) to set the zero offset.
 *
 * @retval 0 on success
 * @retval negative error code on failure
 */
int initialize_load_cell(void);


/**
 * @brief Perform tare (zero offset) for the NAU7802 load cell.
 *
 * This function repeatedly samples the load cell sensor for a number of times
 * (specified by @p readings), calculates the average, and sets it as the zero offset.
 * Use this function to recalibrate the zero point (tare) at any time.
 *
 * @param dev      Pointer to the NAU7802 device structure
 * @param readings Number of samples to average (recommended: 5~20)
 *
 * @retval The calculated zero offset value on success
 * @retval negative error code on failure
 */
int nau7802_tare(const struct device *dev, uint8_t readings);

#ifdef __cplusplus
}
#endif

#endif // LOADCELL_H