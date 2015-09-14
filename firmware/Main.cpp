/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/


// 標準Cライブラリ関連
#include <string.h>

// Arduinoライブラリ関連
#include <EEPROM.h>
#include <Wire.h>

// 独自ライブラリ関連
#include "Main.h"
#include "System.h"
#include "JointController.h"
#include "MotionController.h"


namespace
{
	PLEN2::System           system;
	PLEN2::Purser           purser;
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_controller);
	PLEN2::Interpreter      interpreter;
}


void PLEN2::setup()
{
	joint_ctrl.loadSettings();
}


void PLEN2::loop()
{
	if (motion_ctrl.playing())
	{
		if (motion_ctrl.frameUpdatable())
		{
			motion_ctrl.frameUpdate();
		}

		if (motion_ctrl.frameUpdateFinished())
		{
			if (motion_ctrl.nextFrameLoadable())
			{
				motion_ctrl.loadNextFrame();
			}
			else
			{
				motion_ctrl.stop();
			}
		}
	}

	if (system.USBSerial().available())
	{
		Purser::readByte(system.USBSerial().available());

		if (Purser::lexAccept())
		{
			Purser::makeTokenLog();
			Purser::execEventHandler();
			Purser::transition();
		}
	}

	if (system.BLESerial().available())
	{
		Purser::readByte(system.BLESerial().available());

		if (Purser::lexAccept())
		{
			Purser::makeTokenLog();
			Purser::execEventHandler();
			Purser::transition();
		}
	}
}