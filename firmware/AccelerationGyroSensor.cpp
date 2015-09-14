/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#include "Arduino.h"

#include "Pin.h"
#include "System.h"
#include "AccelerationGyroSensor.h"

#define _DEBUG false


namespace
{
	PLEN2::System system;

	template<typename T>
	void endianCast(T& value)
	{
		char  temp;
		char* filler = (char*)&value;

		for (int index = 0; index < (sizeof(T) / 2); index++)
		{
			temp = filler[sizeof(T) - 1 - index];
			filler[sizeof(T) - 1 - index] = filler[index];
			filler[index] = temp;
		}
	}
}

void PLEN2::AccelerationGyroSensor::sampling()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::sampling()"));
	#endif

	digitalWrite(Pin::RS485_TXD(), HIGH);
	system.BLESerial().write('<');

	digitalWrite(Pin::RS485_TXD(), LOW);
	char  read_count = 0;
	char* filler = (char*)_value;

	while (true)
	{
		if (system.BLESerial().available())
		{
			filler[read_count++] = system.BLESerial().read();
		}

		if (read_count == (BUFFER_LENGTH() * sizeof(int)))
		{
			system.BLESerial().read();

			for (int index = 0; index < BUFFER_LENGTH(); index++)
			{
				endianCast(_value[index]);
			}

			break;
		}
	}
}

int PLEN2::AccelerationGyroSensor::getAccelerationX()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getAccelerationX()"));
	#endif

	return _value[0];
}

int PLEN2::AccelerationGyroSensor::getAccelerationY()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getAccelerationY()"));
	#endif

	return _value[1];
}

int PLEN2::AccelerationGyroSensor::getAccelerationZ()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getAccelerationZ()"));
	#endif

	return _value[2];
}

int PLEN2::AccelerationGyroSensor::getGyroRoll()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getGyroRoll()"));
	#endif

	return _value[3];
}

int PLEN2::AccelerationGyroSensor::getGyroPitch()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getGyroPitch()"));
	#endif

	return _value[4];
}

int PLEN2::AccelerationGyroSensor::getGyroYaw()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getGyroYaw()"));
	#endif

	return _value[5];
}

void PLEN2::AccelerationGyroSensor::dump()
{
	sampling();

	system.outputSerial().println(F("{"));

	system.outputSerial().print(F("\t\"Acc X\": "));
	system.outputSerial().print(getAccelerationX());
	system.outputSerial().println(F(","));

	system.outputSerial().print(F("\t\"Acc Y\": "));
	system.outputSerial().print(getAccelerationY());
	system.outputSerial().println(F(","));

	system.outputSerial().print(F("\t\"Acc Z\": "));
	system.outputSerial().print(getAccelerationZ());
	system.outputSerial().println(F(","));

	system.outputSerial().print(F("\t\"Gyro Roll\": "));
	system.outputSerial().print(getGyroRoll());
	system.outputSerial().println(F(","));

	system.outputSerial().print(F("\t\"Gyro Pitch\": "));
	system.outputSerial().print(getGyroPitch());
	system.outputSerial().println(F(","));

	system.outputSerial().print(F("\t\"Gyro Yaw\": "));
	system.outputSerial().println(getGyroYaw());

	system.outputSerial().println(F("}"));
}