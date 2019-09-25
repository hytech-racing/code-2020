#include <math.h>
//#include "gtsr_debug.h"
#include "soc_table.h"
#include "soc.h"

soc_t bat;
soc_t *battery = &bat;

float soc_lut[SOC_N_POINTS] = SOC_LUT;

int soc_init(soc_t *soc, float min_voltage, uint64_t time_ms) {
    if (!soc) {
        return -1;
    }

    soc->q_net = 0.0f;
    soc->last_update_time = time_ms;
    soc->initial_soc = soc_lookup(min_voltage) * SOC_PACK_AH * 0.01;
    return 0;
}

void soc_update(soc_t *soc, float current, uint64_t time_ms) {
    soc->q_net += (time_ms - soc->last_update_time) / 3600000.0f * current;
    soc->last_update_time = time_ms;
}

float soc_lookup(float voltage) {
    // Binary search
    int16_t left = 0;
    int16_t right = SOC_N_POINTS - 1;
    int16_t mid = 0;
    while (left <= right) {
        mid = floor((left + right) / 2);
        
        if (soc_lut[mid] < voltage) {
            left = mid + 1;
        } else if (soc_lut[mid] > voltage) {
            right = mid - 1;
        } else {
            break;
        }
    }

    float val = 0.0f;
    float matched_voltage = soc_lut[mid];

    // Linear interp
    if (mid < SOC_N_POINTS - 1 && voltage > matched_voltage) {
        val = (1.0/(SOC_N_POINTS-1)) / (soc_lut[mid+1] - matched_voltage) * (voltage - matched_voltage);
    } else if (mid > 0 && voltage < matched_voltage) {
        val = (1.0/(SOC_N_POINTS-1)) / (matched_voltage - soc_lut[mid-1]) * (voltage - matched_voltage);
    }

    val += (mid) / (float) (SOC_N_POINTS - 1);

    return val * 100;
}

void setup() {
  Serial.begin(9600);
  float minVoltage = 3.54;
  soc_init(battery, minVoltage, millis());
  Serial.print(battery->initial_soc);
  Serial.print(" Amp Hours, ");
  Serial.print(soc_lookup(minVoltage));
  Serial.println("%");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  double current = 5;
  soc_update(battery, current, millis());
  Serial.print(battery->initial_soc - battery->q_net);
  Serial.print(" Amp Hours, ");
  Serial.print((battery->initial_soc - battery->q_net) * 100 / 22);
  Serial.println("%");
  delay(1000);
}
