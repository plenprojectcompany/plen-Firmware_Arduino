/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

// Arduinoライブラリ
#include "Arduino.h"

// 独自ライブラリ
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

	/*!
		@note
		Pin::RS485_TXD()をHIGHにすることで、データの流れを"サーボ基盤"→"頭基板"と変更する。
		頭基板のシリアルに任意のデータを送ると、2byte, ビッグエンディアンのバイト列で各
		センサ値を返却してくる。

		これを受信するために、データの流れを即座に"頭基板"→"サーボ基盤"と変更する必要がある。
	*/
	digitalWrite(Pin::RS485_TXD(), HIGH);
	system.BLESerial().write('<');

	digitalWrite(Pin::RS485_TXD(), LOW);
	char  read_count = 0;
	char* filler = (char*)m_values;

	while (true)
	{
		if (system.BLESerial().available())
		{
			filler[read_count++] = system.BLESerial().read();
		}

		if (read_count == (BUFFER_LENGTH() * sizeof(int)))
		{
			// @attention '\n'の読み飛ばしのために必須
			system.BLESerial().read();

			for (int index = 0; index < BUFFER_LENGTH(); index++)
			{
				endianCast(m_values[index]);
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

	return m_values[0];
}

int PLEN2::AccelerationGyroSensor::getAccelerationY()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getAccelerationY()"));
	#endif

	return m_values[1];
}

int PLEN2::AccelerationGyroSensor::getAccelerationZ()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getAccelerationZ()"));
	#endif

	return m_values[2];
}

int PLEN2::AccelerationGyroSensor::getGyroRoll()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getGyroRoll()"));
	#endif

	return m_values[3];
}

int PLEN2::AccelerationGyroSensor::getGyroPitch()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getGyroPitch()"));
	#endif

	return m_values[4];
}

int PLEN2::AccelerationGyroSensor::getGyroYaw()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::getGyroYaw()"));
	#endif

	return m_values[5];
}

void PLEN2::AccelerationGyroSensor::dump()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : AccelerationGyroSensor::dump()"));
	#endif

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