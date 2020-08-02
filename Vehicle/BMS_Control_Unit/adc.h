#include <FaultCounter.h>

extern BMS_status bms_status;
FaultCounter current_fc(CURRENT_FAULT_THRESHOLD);

class HT_ADC {
    public: 
        HT_ADC() : ADC(ADC_CS) {}
        void init() { 
            ADC.read_adc(0); // TODO isoSPI doesn't work until some other SPI gets called. This is a placeholder until we fix the problem
        }
        void process() {
            //Process shutdown circuit measurements
            bms_status.set_shutdown_g_above_threshold(read_adc(CH_SHUTDOWN_G) > SHUTDOWN_HIGH_THRESHOLD);
            bms_status.set_shutdown_h_above_threshold(read_adc(CH_SHUTDOWN_H) > SHUTDOWN_HIGH_THRESHOLD);

            // Process current measurement
            bms_status.set_current(get_current());
            bms_status.set_charge_overcurrent(false); // RESET these values, then check below if they should be set again
            bms_status.set_discharge_overcurrent(false);
            
            if (bms_status.get_current() < CHARGE_CURRENT_CONSTANT_HIGH && !MODE_ADC_IGNORE)
                bms_status.set_charge_overcurrent(current_fc.update(true));
            else if (bms_status.get_current() > DISCHARGE_CURRENT_CONSTANT_HIGH && !MODE_ADC_IGNORE)
                bms_status.set_discharge_overcurrent(current_fc.update(true));
            else
                current_fc.update(false);

            #ifdef HYTECH_LOGGING_EN
                if (current_fc.fault()) {
                    if (bms_status.get_current() < CHARGE_CURRENT_CONSTANT_HIGH)
                        Serial.println("CHARGE CURRENT HIGH FAULT!!!!!!!!!!!!!!!!!!!");
                    else 
                        Serial.println("DISCHARGE CURRENT HIGH FAULT!!!!!!!!!!!!!!!!!!!");
                }
            #endif
        }

    private:
        ADC_SPI ADC; // ADC Declaration

        int16_t get_current() {
            /*
            * Current sensor: ISB-300-A-604
            * Maximum positive current (300A) corresponds to 4.5V signal
            * Maximum negative current (-300A) corresponds to 0.5V signal
            * 0A current corresponds to 2.5V signal
            *
            * voltage = read_adc() * 5 / 4095
            * current = (voltage - 2.5) * 300 / 2
            */
            double voltage = read_adc(CH_CUR_SENSE) / (double) 819;
            double current = (voltage - 2.5) * (double) 150;
            return (int16_t) (current * 100); // Current in Amps x 100
        }

        // Helper function reads from ADC then sets SPI settings such that isoSPI will continue to work
        int read_adc(int channel) {
            noInterrupts(); // Since timer interrupt triggers SPI communication, we don't want it to interrupt other SPI communication
            int retval = ADC.read_adc(channel);
            interrupts();
            spi_enable(SPI_CLOCK_DIV16); // Reconfigure 1MHz SPI clock speed after ADC reading so LTC communication is successful
            return retval;
        }
};