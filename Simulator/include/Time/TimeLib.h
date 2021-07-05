#pragma once
#include <ctime>
#include <stdint.h>
#include "HTException.h"

typedef enum { timeNotSet, timeNeedsSync, timeSet } timeStatus_t;
typedef time_t(*getExternalTime)();

class teensy3_clock_class {
public:
	inline static time_t get() { if (!valid) throw HTException("RTC Exception", "RTC not configured"); return std::time(0); }
	inline static void set(time_t t) {}
	inline static void teardown() { valid = false; }

	friend void setSyncProvider(getExternalTime sync);

private:
	static time_t rtcTime;
	static bool valid;
};

extern teensy3_clock_class Teensy3Clock;

inline uint16_t year()		{ time_t t = Teensy3Clock.get(); return localtime(&t)->tm_year + 1900;	}
inline uint16_t month()		{ time_t t = Teensy3Clock.get(); return localtime(&t)->tm_mon;			}
inline uint16_t day()		{ time_t t = Teensy3Clock.get(); return localtime(&t)->tm_mday;			}
inline uint16_t hour()		{ time_t t = Teensy3Clock.get(); return localtime(&t)->tm_hour;			}
inline uint16_t minute()	{ time_t t = Teensy3Clock.get(); return localtime(&t)->tm_min;			}
inline uint16_t second()	{ time_t t = Teensy3Clock.get(); return localtime(&t)->tm_sec;			}

#define RTC_TSR (second())
#define RTC_TPR (0)

inline void setSyncProvider(getExternalTime sync) {
	Teensy3Clock.valid = (sync == teensy3_clock_class::get);
}

timeStatus_t timeStatus() {
	return Teensy3Clock.get() ? timeSet : timeNotSet;
}
