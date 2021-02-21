#pragma once
#include <string.h>
#include <cstdint>

#pragma pack(push,1)

// @Parseclass
class MCU_GPS_readings {
public:
    MCU_GPS_readings() = default;
    MCU_GPS_readings(uint8_t buf[8]) { load(buf); }

    inline void load(uint8_t buf[])     { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[])    { memcpy(buf, this, sizeof(*this)); }

    inline int32_t get_latitude()   const { return latitude; }
    inline int32_t get_longitude()  const { return longitude; }

    inline void set_latitude(int32_t latitude)      { this->latitude = latitude; }
    inline void set_longitude(int32_t longitude)    { this->longitude = longitude; }

#ifdef HT_DEBUG_EN
    void print() {
        Serial.println("\n\nGPS Readings");
        Serial.println("-----------");
    	Serial.print("Latitude (x100000): "); Serial.println(get_latitude());
        Serial.print("Longitude (x100000): "); Serial.println(get_longitude());
    }
#endif

private:
    int32_t latitude; // @Parse @Scale(100000)
    int32_t longitude; // @Parse @Scale(100000)
};

#pragma pack(pop)