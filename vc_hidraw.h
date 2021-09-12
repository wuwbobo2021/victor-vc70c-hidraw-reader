/*
    victor-vc70c-hidraw-reader Version 1.00
    A decoding program for VICTOR VC70C/86C/86D Multimeter as computer device.
    Copyright (C) 2021 wuwbobo2021 <wuwbobo@outlook.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    This program is NOT licensed in LGPL. For personal use only,
    except as permitted by XI'AN BEICHENG ELECTRONICS CO.,LTD.
    
    It was my father's effort to decode the HID data from the VC70C
    multimeter, for which he had published an VB6 program (binary)
    with Socket interface:
    <https://www.shoudian.org/thread-211033-1-1.html>
    
    A decade later when I tried to write the program on Linux, I found
    similar codes on Github including <https://github.com/mvneves/victor70c>,
    so I decided to rewrite it in another form with some minor features included.
    <https://github.com/wuwbobo2021/victor-vc70c-hidraw-reader>
*/

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
	bool ol; //open-loop
	bool low_battery_voltage; //battery of multimeter itself
	
	char str[21]; // 1 (' ' or '-') + 5 (decimal number) + 1 (' ') + 3 (numeric unit) +
	              // 3 (unit) + 3 (DC/AC) + 4 (low capacity indication) + 1 ('\0')
} vc_multimeter_reading;

extern bool open_vc_multimeter_hidraw(); //return false if device not found
extern void close_vc_multimeter_hidraw(); //close hidraw handle
extern bool read_vc_multimeter_hidraw(vc_multimeter_reading* reading); //block until data received, return false if failed

#ifdef __cplusplus
}
#endif

#endif
