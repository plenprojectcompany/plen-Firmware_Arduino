/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/


// Arduinoライブラリ
#include <EEPROM.h>
#include <Wire.h>

// 独自ライブラリ
#include "AccelerationGyroSensor.h"
#include "Interpreter.h"
#include "JointController.h"
#include "MotionController.h"
#include "PurserCombinator.h"
#include "System.h"


namespace
{
	PLEN2::AccelerationGyroSensor sensor;
	PLEN2::JointController        joint_ctrl;
	PLEN2::MotionController       motion_ctrl(joint_controller);
	PLEN2::Interpreter            interpreter(motion_ctrl);
	PLEN2::PurserCombinator       combinator;
	PLEN2::System                 system;
}


/*!
	@brief 初期化メソッド

	必要な初期化処理はこちらに記述してください。

	@attention
	デジタルピンの出力は初期レベルを与えない場合不定となります。
	モード設定を行うようなピンの初期化は必ず行ってください。
*/
void setup()
{
	joint_ctrl.loadSettings();
}


/*!
	@brief メインポーリングループ

	繰り返し実行したい処理はこちらに記述してください。

	@attention
	動作を強制終了したい場合、exit()命令を使用する必要があります。
	return命令では、再度ループが実行されてしまいます。
*/
void loop()
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
				if (interpreter.ready())
				{
					interpreter.popCode();
				}
				else
				{
					motion_ctrl.stop();
				}
			}
		}
	}

	if (system.USBSerial().available())
	{
		combinator.readByte(system.USBSerial().available());

		if (combinator.accept())
		{
			combinator.execEventHandler();
			combinator.transition();
		}
	}

	if (system.BLESerial().available())
	{
		combinator.readByte(system.BLESerial().available());

		if (combinator.accept())
		{
			combinator.execEventHandler();
			combinator.transition();
		}
	}
}