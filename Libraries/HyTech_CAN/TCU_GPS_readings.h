#pragma once

#include <string.h>
#include <cstdint>

#pragma pack(push,1)

class TCU_GPS_readings {
public:
    TCU_GPS_readings() = default;
    TCU_GPS_readings(const uint8_t buf[8]) { load(buf); }

    inline void load(const uint8_t buf[])  { memcpy(this, buf, sizeof(*this)); }
    inline void write(uint8_t buf[]) const { memcpy(buf, this, sizeof(*this)); }

    inline int32_t get_latitude()  const { return latitude; }
    inline int32_t get_longitude() const { return longitude; }

    inline void set_latitude(int32_t latitude)   { this->latitude = latitude; }
    inline void set_longitude(int32_t longitude) { this->longitude = longitude; }
private:
    int32_t latitude;
    int32_t longitude;
};

#pragma pack(pop)