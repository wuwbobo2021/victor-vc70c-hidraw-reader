// victor-vc70c-hidraw-reader 1.00

// Without any warranty.
// In the purpose of bug reporting and version identification,
// anyone who modifies the code should remain this comment and add
// a record with an email address and the modifying date included.

// Version 1.00 (2021-9-8, wuwbobo@outlook.com)
// Original version.

#ifndef VC_HIDRAW_H

#define VC_HIDRAW_H

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

typedef enum vc_multimeter_unit{
    VC_Unit_None = 0,
    VC_Unit_Volt,
    VC_Unit_Ampere,
    VC_Unit_Ohm,
    VC_Unit_Celsius,
    VC_Unit_Fahrenheit,
    VC_Unit_Hertz,
    VC_Unit_Farad
} vc_multimeter_unit;

typedef struct vc_multimeter_reading{
	float value;
	bool minus;
	vc_multimeter_unit unit;
	bool AC;
	bool percent_unit;
	bool ol;
	bool low_battery_voltage;
	
	char str[21]; // 1 (' ' or '-') + 5 (decimal number) + 1 (' ') + 3 (numeric unit) +
	              // 3 (unit) + 3 (DC/AC) + 4 (low capacity indication) + 1 ('\0')
} vc_multimeter_reading;

extern bool open_vc_multimeter_hidraw();
extern void close_vc_multimeter_hidraw();
extern bool read_vc_multimeter_hidraw(vc_multimeter_reading* reading);

#ifdef __cplusplus
}
#endif

#endif
