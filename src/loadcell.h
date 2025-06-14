#ifndef LOADCELL_H
#define LOADCELL_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

int loadcell_init(const struct device *dev);
int loadcell_read_grams(float *grams);

#ifdef __cplusplus
}
#endif

#endif // LOADCELL_H