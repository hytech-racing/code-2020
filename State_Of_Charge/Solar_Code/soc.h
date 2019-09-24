#ifndef __SOC_H
#define __SOC_H

#include <stdint.h>

#include "soc_table.h"

#define SOC_LUT         SOC_LUT_LG_HG2
#define SOC_N_POINTS    (SOC_LUT_LG_HG2_LEN)

#define SOC_PACK_AH     36.0f

typedef struct {
    // Net charge consumed since initialization in Ah
    float       q_net;
    // Calculated SoC of the pack on startup in Ah
    float       initial_soc;
    uint64_t    last_update_time;
} soc_t;

// Lookup SoC value based on min voltage at initialization
int soc_init(soc_t *soc, float min_voltage, uint64_t time_ms);

// Coulomb count SoC based on net current
void soc_update(soc_t *soc, float current, uint64_t time_ms);

// Lookup SoC for a given voltage
float soc_lookup(float voltage);

#endif