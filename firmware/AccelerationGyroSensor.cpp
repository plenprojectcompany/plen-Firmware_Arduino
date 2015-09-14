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

namespace
{
	PLEN2::System system;
}

void PLEN2::AccelerationGyroSensor::sampling()
{
	digitalWrite(Pin::RS485_TXD(), HIGH);
	system.BLESerial().write('<');

	digitalWrite(Pin::RS485_TXD(), LOW);
	char read_count = 0;

	while (true)
	{
		if (system.BLESerial().available())
		{
			_value[read_count++] = system.BLESerial().read();
		}

		if (get_count == BUFFER_LENGTH())
		{
			break;
		}
	}
}

int PLEN2::AccelerationGyroSensor::getAccelerationX()
{
	return _value[0];
}

int PLEN2::AccelerationGyroSensor::getAccelerationY()
{
	return _value[1];
}

int PLEN2::AccelerationGyroSensor::getAccelerationZ()
{
	return _value[2];
}

int PLEN2::AccelerationGyroSensor::getGyroRoll()
{
	return _value[3];
}

int PLEN2::AccelerationGyroSensor::getGyroPitch()
{
	return _value[4];
}

int PLEN2::AccelerationGyroSensor::getGyroYaw()
{
	return _value[5];
}