#ifndef LOADCELL_H
#define LOADCELL_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

int initialize_load_cell(void);

#ifdef __cplusplus
}
#endif

#endif // LOADCELL_H