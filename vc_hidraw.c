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
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <dirent.h>

#include "vc_hidraw.h"

typedef unsigned char BYTE;

const int VC_HID_Vendor_Number = 0x1244;
const int VC_HID_Product_Number = 0xD237;

const BYTE VC_HID_Valid_Buffer_Size = 13;

// digits of bits in a BYTE are 0~7, in which 7th bit is the highest bit

const BYTE VC_HID_Unit_Byte = 0x8;

const BYTE VC_HID_Polarity_Byte = 0x4;
const BYTE VC_HID_Minus_Bit = 0; //0: positive  1: minus

const BYTE VC_HID_DCAC_Byte = 0x1;
const BYTE VC_HID_DC_Bit = 5; //1: DC  0: AC

const BYTE VC_HID_OL_Byte = 0x9;
const BYTE VC_HID_Value_Byte[4] = {0xA, 0x3, 0x9, 0x6};
const BYTE VC_HID_Numeric_Unit_Byte = 0xC;
const BYTE VC_HID_Decimal_Byte = 0x5;

const BYTE VC_HID_Signs_Byte = 0x2;
const BYTE VC_HID_Low_Battery_Voltage_Bit = 5; //0: multimeter battery has low capacity
const BYTE VC_HID_Unit_n_Bit = 6; //0: numeric unit is n

enum{
	VC_HID_Unit_V = 0x6A,
	VC_HID_Unit_A = 0x6B,
	VC_HID_Unit_Ohm = 0x6D,
	VC_HID_Unit_F = 0x89,
	VC_HID_Unit_degC = 0xA9,
	VC_HID_Unit_degF = 0xE9,
	VC_HID_Unit_Hz = 0x79	
};

enum{
	VC_HID_Numeric_Unit_mu = 0x6A,
	VC_HID_Numeric_Unit_m = 0x6B,
	VC_HID_Numeric_Unit_k = 0x6D,
	VC_HID_Numeric_Unit_M = 0x71,
	VC_HID_Numeric_Unit_per = 0xA9
};

const BYTE VC_HID_OL = 0xBF;

#define N (0xFF)
const BYTE VC_HID_Digit_Byte_Low[4] = {0x7, 0x1, 0xF, 0x1};
const BYTE VC_HID_Digit_Decoding[4][16]
     // 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F (from high-order 4 bits)
	= {{9, 5, N, 3, N, 7, N, 0, 8, 4, N, 2, N, 6, N, 1},  // buf[0xA], low: 0x7
	   {9, 5, N, 3, N, 7, N, 0, 8, 4, N, 2, N, 6, N, 1},  // buf[0x3], low: 0x1
	   {5, N, 3, N, 7, N, 0, 8, 4, N, 2, N, 6, N, 1, 9},  // buf[0x9], low: 0xF  BF: OL
	   {1, 9, 5, N, 3, N, 7, N, 0, 8, 4, N, 2, N, 6, N}}; // buf[0x6], low: 0x1

const BYTE VC_HID_Decimal_Decoding[16] // e+0, 1, 2, 3
	=  {0, N, N, N, N, N, N, N, 3, N, 2, N, 1, N, N, N};  // buf[0x5], low: 0x4


bool byte_bit(BYTE b, BYTE digit){
	if (digit < 0 || digit > 7) return false;
	return ((b >> digit) & 0x01);
}

int vc_fd; bool vc_found;

bool open_vc_multimeter_hidraw(){
	if (vc_found) return true;
	
	DIR* dir = opendir("/dev");
	struct dirent* ent;
	char devpath[512];
	while (ent = readdir(dir)){ //read next entry name
		if (ent->d_type != DT_CHR) continue; //it should be char device
		if (strncmp(ent->d_name, "hidraw", 6) == 0){
			snprintf(devpath, sizeof(devpath), "%s/%s", "/dev", ent->d_name);
			vc_fd = open(devpath, O_RDONLY);
			if (vc_fd < 0) return false; //no permmission

			struct hidraw_devinfo raw_info;
			if (ioctl(vc_fd, HIDIOCGRAWINFO, &raw_info) == -1) return false; //read hidraw info
			if (   raw_info.vendor == VC_HID_Vendor_Number
			    && (raw_info.product & 0xFFFF) == VC_HID_Product_Number)
				return (vc_found = true);
		
			close(vc_fd); //if found, it will not close
		}
	}
	
	return false;
}

void close_vc_multimeter_hidraw(){
	if (vc_found) close(vc_fd);
}

bool read_vc_multimeter_hidraw(vc_multimeter_reading* reading){	
	if (! vc_found) return false;
	
	long int res; BYTE buf[HIDRAW_BUFFER_SIZE];
	res = read(vc_fd, buf, HIDRAW_BUFFER_SIZE);
	if (res < VC_HID_Valid_Buffer_Size) return false;
	
	reading->minus = byte_bit(buf[VC_HID_Polarity_Byte], VC_HID_Minus_Bit);
	reading->AC = ! byte_bit(buf[VC_HID_DCAC_Byte], VC_HID_DC_Bit);
	
	reading->str[0] = (reading->minus? '-' : ' ');
	
	reading->value = 0;
	if (buf[VC_HID_OL_Byte] == VC_HID_OL){
		reading->ol = true; reading->value = 0;
		strcpy(reading->str + 1, "OL ");
	} else {
		reading->ol = false;
	
		BYTE dg; float level = 1; //dg is number, not real character
		BYTE chdg[4];
		for (char i = 0; i < 4; i++){
			if ((buf[VC_HID_Value_Byte[i]] & 0x0F) != VC_HID_Digit_Byte_Low[i])
				return false; //invalid data
			
			dg = VC_HID_Digit_Decoding[i][buf[VC_HID_Value_Byte[i]] >> 4];
			if (dg == N) return false; //invalid data
			
			reading->value += dg * level;
			chdg[i] = dg + 0x30; //transfer to ASCII character
			
			level /= 10.0;
		} // now reading->value is x.xxx
		
		BYTE d = VC_HID_Decimal_Decoding[buf[VC_HID_Decimal_Byte] >> 4];
		for (BYTE i = 0; i < d; i++) reading->value *= 10.0;
		
		BYTE j = 0;
		for (BYTE i = 0; i < 5; i++){ //count of the float number chars is 5
			if (i == d + 1) reading->str[1 + i] = '.'; //'.' is in d + 1 position within 0~4 chars
			else {reading->str[1 + i] = chdg[j]; j++;}
		}
		reading->str[6] = ' '; reading->str[7] = '\0';
		
		reading->percent_unit = false;
		switch ((int) buf[VC_HID_Numeric_Unit_Byte]){
			case VC_HID_Numeric_Unit_mu:
				reading->value /= 1000*1000;
				strcat(reading->str, "μ"); break; //may cost 3 bytes in the string
			case VC_HID_Numeric_Unit_m:
				reading->value /= 1000;
				strcat(reading->str, "m"); break;
			case VC_HID_Numeric_Unit_k: 
				reading->value *= 1000;
				strcat(reading->str, "k"); break;
			case VC_HID_Numeric_Unit_M:
				reading->value *= 1000*1000;
				strcat(reading->str, "M"); break;
		
			case VC_HID_Numeric_Unit_per:
				reading->percent_unit = true;
				strcat(reading->str, "%"); break;
		}
	
		if (reading->minus) reading->value *= -1;
	}
	
	BYTE t = buf[VC_HID_Signs_Byte];
	reading->low_battery_voltage = ! byte_bit(t, VC_HID_Low_Battery_Voltage_Bit);
	if (! byte_bit(t, VC_HID_Unit_n_Bit)){
		reading->value /= 1000*1000*1000;
		strcat(reading->str, "n");
	}
	
	switch ((int) buf[VC_HID_Unit_Byte]){
		case VC_HID_Unit_V:
			reading->unit = VC_Unit_Volt;
			strcat(reading->str, "V"); break;
		case VC_HID_Unit_A:
			reading->unit = VC_Unit_Ampere;
			strcat(reading->str, "A"); break;
		case VC_HID_Unit_Ohm:
			reading->unit = VC_Unit_Ohm;
			strcat(reading->str, "Ω"); break; //may use 3 bytes
		case VC_HID_Unit_F:
			reading->unit = VC_Unit_Farad;
			strcat(reading->str, "F"); break;
		case VC_HID_Unit_degC:
			reading->unit = VC_Unit_Celsius;
			strcat(reading->str, "℃"); break; //may use 3 bytes
		case VC_HID_Unit_degF:
			reading->unit = VC_Unit_Fahrenheit;
			strcat(reading->str, "℉"); break; //may use 3 bytes
		case VC_HID_Unit_Hz:
			reading->unit = VC_Unit_Hertz;
			strcat(reading->str, "Hz"); break; //2 bytes
		
		default: reading->unit = VC_Unit_None;
	}
	
	if (reading->unit == VC_Unit_Volt || reading->unit == VC_Unit_Ampere)
		strcat(reading->str, reading->AC ? " AC" : " DC"); //3 bytes
	
	if (reading->low_battery_voltage)
		strcat(reading->str, " BAT"); //4 bytes
	
	return true;
}

