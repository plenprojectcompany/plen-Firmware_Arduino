/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/

#define _DEBUG false

// Arduinoライブラリ
#include "Arduino.h"

// 独自ライブラリ
#include "Pin.h"
#include "System.h"
#include "AccelerationGyroSensor.h"
#include "JointController.h"
#include "MotionController.h"
#include "Soul.h"


namespace
{
	enum {
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
		EOE
	};

	long acc_backup[EOE] = { 0 };

	PLEN2::System system;
}


PLEN2::Soul::Soul(AccelerationGyroSensor& sensor, MotionController& motion_ctrl)
{
	m_sensor_ptr      = &sensor;
	m_motion_ctrl_ptr = &motion_ctrl;

	for (int index = 0; index < MOTION_SUM(); index++)
	{
		m_motions[index] = MOTIONS_SLOT_BEGIN() + index;
	}

	m_before_call_sec = millis() / 1000;
	m_next_call_sec   = m_before_call_sec + BASE_INTERVAL_SEC();
}


void PLEN2::Soul::logging()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : Soul::logging()"));
	#endif

	/*!
		@note
		既にいずれかのシリアルへデータが到着している場合、
		コマンドラインの撹乱を防ぐために処理を抜ける。
	*/
	if (   system.BLESerial().available()
		|| system.USBSerial().available() )
	{
		return;
	}

	m_sensor_ptr->sampling();

	acc_backup[X_AXIS] = (char)m_sensor_ptr->getAccX();
	acc_backup[Y_AXIS] = (char)m_sensor_ptr->getAccY();
	acc_backup[Z_AXIS] = (char)m_sensor_ptr->getAccZ();
}


void PLEN2::Soul::userActionInputed()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : Soul::userActionInputed()"));
	#endif

	m_before_call_sec = millis() / 1000;
	m_next_call_sec   = m_before_call_sec + BASE_INTERVAL_SEC() + analogRead(0);
}


void PLEN2::Soul::action()
{
	#if _DEBUG
		system.outputSerial().println(F("=== in fuction : Soul::action()"));
	#endif

	m_motion_ctrl_ptr->play(m_motions[random(MOTION_SUM())]);
}