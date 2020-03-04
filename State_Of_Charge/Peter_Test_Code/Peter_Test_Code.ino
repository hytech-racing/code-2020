#include <math.h>
#include <ADC_SPI.h>
#include "soc_table.h"
#include "soc.h"

soc_t bat;
soc_t *battery = &bat;
ADC_SPI adc;

double voltage_conversion_factor      = 5.033333333333333333 / 4095;   // determined by testing
double current_conversion_factor      = 150   / 1.500;  // L01Z150S05 current sensor outputs 4 V at 150 A, and 2.5 V at 0 A
long int    current_offset            = 0;
int    calibration_reads              = 500;    

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

double getBatteryCurrent() {
  // Method to read the cell current in Amps
  // Arguments: channel (Note: in the code, channels are numbered 0-3, when on the board they are 4-7 which are designated as current sense)
  //double voltage_reading = ((double) adc.read_adc(4)) * voltage_conversion_factor;
  double voltage_reading = (double) analogRead(4)* 500.0 / 1024;
  voltage_reading = voltage_reading - current_offset;
  double current_reading = voltage_reading;
  //double current_reading = 0.997 * voltage_reading + 0.2621;
  return current_reading;
}

void setup() {
  pinMode(A3, INPUT);
  Serial.begin(9600);
  adc = ADC_SPI();

  for (int j = 0; j < calibration_reads; j++){
    current_offset = current_offset + (analogRead(4)* 500.0 / 1024);
    delay(50);
  }
  current_offset = round(current_offset / (double)calibration_reads);

  
  float minVoltage = 4.16;
  soc_init(battery, minVoltage, millis());
  Serial.print(battery->initial_soc);
  Serial.print(" Amp Hours, ");
  Serial.print(soc_lookup(minVoltage));
  Serial.println("%");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //double current = analogRead(15)* 500.0 / 1024;
  double current = getBatteryCurrent();
  Serial.print("Current: ");
  Serial.println(current);
  soc_update(battery, current, millis());
  Serial.print(battery->initial_soc - battery->q_net);
  Serial.print(" Amp Hours, ");
  Serial.print((battery->initial_soc - battery->q_net) * 100 / 22);
  Serial.println("%");
  delay(1000);
}
