/*
 HaLowAP.cpp - HaLow AP mode library for esp32

 Copyright (c) 2024 Heltec AutoMation.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "Arduino.h"

extern RTC_DATA_ATTR uint32_t storedlicense[4];
extern RTC_DATA_ATTR uint32_t correctlicense[4];

void check_input_license()
{
	int i;
	i=0;
	char rxData[50];
	uint8_t inputlicense[32];
	uint32_t timestart;
	timestart = millis();
	Serial.flush();
	  while((millis()-timestart)<1000)
	  {
		if(Serial.available()>0)
		{
		  rxData[i++]=(uint8_t)Serial.read();
		}
		if(i==41)
		{
		  break;
		}
	  }
	  rxData[i]=0;
	  if(i!=41||rxData[8]!='=')
	  {
		return;
	  }
	  for(int j=0;j<32;j++)
	  {
		inputlicense[j]=rxData[j+9];
		if(inputlicense[j]>='a')
		{
		  inputlicense[j]-=32;
		}
		if(inputlicense[j]<='9')
		{
		  inputlicense[j]-='0';
		}
		else
		{
		  inputlicense[j]-=('A'-10);
		}
	  }
	
	  for(int j=0;j<4;j++)
	  {
		storedlicense[j]=0;
		for(int k=0;k<8;k++)
		{
		 storedlicense[j]|=(uint32_t)inputlicense[8*j+k]<<(28-k*4);
		}
	  }
}