#ifndef LOADCELL_H
#define LOADCELL_H

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	float battery;
	float weight;
	float accel_mag;
} sensors_t;

/* Global sensor data */
extern sensors_t sensors;

// int loadcell_init(const struct device *dev);
// int loadcell_read_grams(float *grams);

int initialize_load_cell(void);
void sensors_read_load_cell(void);

#ifdef __cplusplus
}
#endif

#endif // LOADCELL_H