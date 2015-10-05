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
#include "Purser.h"
#include "PurserCombinator.h"
#include "System.h"

// デバッグマクロ
#define _DEBUG true


namespace
{
	// コアインスタンス
	PLEN2::AccelerationGyroSensor sensor;
	PLEN2::JointController        joint_ctrl;
	PLEN2::MotionController       motion_ctrl(joint_ctrl);
	PLEN2::Interpreter            interpreter(motion_ctrl);
	PLEN2::System                 system;

	// イベントハンドラ
	void angleDiff()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : angleDiff()"));
		#endif
	}

	void angle()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : angle()"));
		#endif
	}

	void homePosition()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : homePosition()"));
		#endif
	}

	void playMotion()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : playMotion()"));
		#endif
	}

	void stopMotion()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : stopMotion()"));
		#endif
	}

	void popCode()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : popCode()"));
		#endif
	}

	void pushCode()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : pushCode()"));
		#endif
	}

	void resetInterpreter()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : resetInterpreter()"));
		#endif
	}

	void setHomeAngle()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : setHomeAngle()"));
		#endif
	}

	void setJointSettings()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : setJointSettings()"));
		#endif
	}

	void setMaxAngle()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : setMaxAngle()"));
		#endif
	}

	void setMotionFrame()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : setMotionFrame()"));
		#endif
	}

	void setMotionHeader()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : setMotionHeader()"));
		#endif
	}

	void setMinAngle()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : setMinAngle()"));
		#endif
	}

	void getJointSettings()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : getJointSettings()"));
		#endif

		joint_ctrl.dump();
	}

	void getMotion()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : getMotion()"));
		#endif
	}

	void getVersionInformation()
	{
		#if _DEBUG
			system.outputSerial().println(F("# in event handler : getVersionInformation()"));
		#endif

		system.dump();
	}

	void (*CONTROLLER_EVENT_HANDLER[])() = {
		angleDiff,
		angle,
		homePosition,
		playMotion,
		stopMotion,
		playMotion,
		stopMotion
	};

	void (*INTERPRETER_EVENT_HANDLER[])() = {
		popCode,
		pushCode,
		resetInterpreter
	};

	void (*SETTER_EVENT_HANDLER[])() = {
		setHomeAngle,
		setMotionHeader, // sanity check.
		setJointSettings,
		setMaxAngle,
		setMotionFrame,
		setMotionHeader,
		setMinAngle
	};

	void (*GETTER_EVENT_HANDLER[])() = {
		getJointSettings,
		getMotion,
		getVersionInformation
	};

	void (**EVENT_HANDLER[])() = {
		CONTROLLER_EVENT_HANDLER,
		INTERPRETER_EVENT_HANDLER,
		SETTER_EVENT_HANDLER,
		GETTER_EVENT_HANDLER
	};

	class Application : public PLEN2::PurserCombinator
	{
	private:
		PLEN2::MotionController::Header m_header;
		PLEN2::MotionController::Frame  m_frame;

	public:
		virtual void afterFook()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in function : Application::afterFook()"));
			#endif

			if (m_state == READY)
			{
				unsigned char header_id = m_purser[READY]->index();
				unsigned char cmd_id    = m_purser[COMMAND_INCOMING]->index();

				EVENT_HANDLER[header_id][cmd_id]();
			}

			if (m_state == )
		}
	};

	Application combinator;
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
	#if _DEBUG
		while (!Serial);

		Serial.println("Debug started.");
		Serial.println("==============");
	#endif

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
		combinator.readByte(system.USBSerial().read());

		if (combinator.accept())
		{
			combinator.transition();

		}
	}

	if (system.BLESerial().available())
	{
		combinator.readByte(system.BLESerial().read());

		if (combinator.accept())
		{
			combinator.transition();
		}
	}
}
