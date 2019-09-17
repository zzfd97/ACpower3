/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#include "Arduino.h"
#include "ACpower3.h"
#include "ACpower3_macros.h"

#if defined(ESP32)

//uint8_t ACpower3::_pinI;
//uint8_t ACpower3::_pinU;
//uint16_t ACpower3::_Izerolevel = 0;
//uint16_t ACpower3::_Uzerolevel = 0;
//uint8_t ACpower3::_phaseQty = 0;

ACpower3::ACpower3()
{
	Pmax = POWER_MAX * 3;		// а надо ли??
	_pinZCross[0] = PIN_ZC0;	// пин подключения детектора нуля.
	_pinTriac[0] = PIN_TR0;		// пин управляющий триаком. 
	_pinZCross[1] = PIN_ZC1;	
	_pinTriac[1] = PIN_TR1;		
	_pinZCross[2] = PIN_ZC2;	
	_pinTriac[2] = PIN_TR2;		
	_pinI[0] = PIN_I0;		// пин датчика тока.
	_pinU[0] = PIN_U0;		// пин датчика напряжения. 
	_pinI[1] = PIN_I1;		
	_pinU[1] = PIN_U1;		
	_pinI[2] = PIN_I2;		
	_pinU[2] = PIN_U2;		
	_useADC = true;
	_ShowLog = true;
	return;
}

ACpower3::ACpower3( uint8_t pinZC0, uint8_t pinTR0, uint8_t pinI0, uint8_t pinU0, \
					uint8_t pinZC1, uint8_t pinTR1, uint8_t pinI1, uint8_t pinU1, \
					uint8_t pinZC2, uint8_t pinTR2, uint8_t pinI2, uint8_t pinU2)
{
	Pmax = ACPOWER3_MAX;		// а надо ли??
	_pinZCross[0] = pinZC0;		// пин подключения детектора нуля.
	_pinTriac[0] = pinTR0;		// пин управляющий триаком. 
	_pinZCross[1] = pinZC1;		
	_pinTriac[1] = pinTR1;		 
	_pinZCross[2] = pinZC2;	
	_pinTriac[2] = pinTR2;	
	_pinI[0] = pinI0;		// пин датчика тока.
	_pinU[0] = pinU0;		// пин датчика напряжения. 
	_pinI[1] = pinI1;		
	_pinU[1] = pinU1;		
	_pinI[2] = pinI2;		
	_pinU[2] = pinU2;		
	_useADC = true;
	_ShowLog = true;
	return;
}

void ACpower3::init()
{ 
	init();
	initADC();
	setRMSratio(ADC_I_RATIO, ADC_U_RATIO);
}

void ACpower3::init(loat Iratio, float Uratio)
{ 
	init();
	initADC();
	setRMSratio(Iratio, Uratio);
}

void ACpower3::init()
{  
	Angle = 0;
	if (_ShowLog)
	{
		Serial.println(F(LIBVERSION));
		PRINTF(" + Pmax: ", Pmax);
	}
	for (int i=0; i<3; i++)
	{
		
		//if (_ShowLog) printConfig(i);
		DELAYx;
		setup_Triac(i);
		DELAYx;
		setup_ZeroCross(i);
		DELAYx;
	}
	return;
}

void ACpower3::initADC()
{
	for (int i=0; i<3; i++)					// TEST!! наверное можно и без этого
	{
		pinMode(_pinI[i], INPUT);           // set pin to input
		//digitalWrite(_pinI[i], HIGH); 
		pinMode(_pinU[i], INPUT);           // set pin to input
		//digitalWrite(_pinU[i], HIGH); 
	}
	
	delay(20);
	setRMSzerolevel(ZEROLEVEL_SAMPLES);
	// setRMSratio(ADC_I_RATIO, ADC_U_RATIO);
	setup_ADC();
}


void ACpower3::stop()
{
	Angle = 0;
	delay(20);
	
	if (_useADC)
	{
		timerStop(timerADC);
		timerDetachInterrupt(timerADC);
	}
	
	for (int i=0; i<3; i++)
	{
		timerStop(timerTriac[i]);
		timerDetachInterrupt(timerTriac[i]);
		detachInterrupt(digitalPinToInterrupt(_pinZCross[i]));
		digitalWrite(_pinTriac[i], LOW);
	}
}




void ACpower3::printConfig(uint8_t i)
{
	Serial.print(F(" . ZeroCross on pin "));
	Serial.print(_pinZCross[i]);
	Serial.print(F(", Triac on pin "));
	Serial.println(_pinTriac[i]);
}

#endif // ESP32
