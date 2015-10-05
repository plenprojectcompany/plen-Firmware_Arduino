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
#define _DEBUG false


namespace
{
	// コアインスタンス
	PLEN2::AccelerationGyroSensor sensor;
	PLEN2::JointController        joint_ctrl;
	PLEN2::MotionController       motion_ctrl(joint_ctrl);
	PLEN2::Interpreter            interpreter(motion_ctrl);
	PLEN2::System                 system;


	// アプリケーションインスタンス
	class Application : public PLEN2::PurserCombinator
	{
	private:
		static bool (Application::*CONTROLLER_EVENT_HANDLER[])();
		static bool (Application::*INTERPRETER_EVENT_HANDLER[])();
		static bool (Application::*SETTER_EVENT_HANDLER[])();
		static bool (Application::*GETTER_EVENT_HANDLER[])();

		static bool (Application::**EVENT_HANDLER[])();

		PLEN2::MotionController::Header m_header_tmp;
		PLEN2::MotionController::Frame  m_frame_tmp;

		bool angleDiff()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::angleDiff()"));
			#endif
		}

		bool angle()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::angle()"));
			#endif
		}

		bool homePosition()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::homePosition()"));
			#endif

			joint_ctrl.loadSettings();
		}

		bool playMotion()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::playMotion()"));
			#endif
		}

		bool stopMotion()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::stopMotion()"));
			#endif
		}

		bool popCode()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::popCode()"));
			#endif

			interpreter.popCode();
		}

		bool pushCode()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::pushCode()"));
			#endif
		}

		bool resetInterpreter()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::resetInterpreter()"));
			#endif

			interpreter.reset();
		}

		bool setHomeAngle()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setHomeAngle()"));
			#endif
		}

		bool setJointSettings()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setJointSettings()"));
			#endif

			joint_ctrl.resetSettings();
		}

		bool setMaxAngle()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMaxAngle()"));
			#endif
		}

		bool setMotionFrame()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMotionFrame()"));
			#endif

			if (m_installing == true)
			{
				readByte('>');
				accept();
				transition();

				readByte('m');
				readByte('f');
				accept();
				transition();

				readByte('0');
				readByte('0');
				readByte('0');
				readByte('0');
			}
		}

		bool setMotionHeader()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMotionHeader()"));
			#endif

			if (m_installing == true)
			{
				readByte('>');
				accept();
				transition();

				readByte('m');
				readByte('f');
				accept();
				transition();
			}
		}

		bool setMinAngle()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::setMinAngle()"));
			#endif
		}

		bool getJointSettings()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::getJointSettings()"));
			#endif

			joint_ctrl.dump();
		}

		bool getMotion()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::getMotion()"));
			#endif

			motion_ctrl.dump(0);
		}

		bool getVersionInformation()
		{
			#if _DEBUG
				system.outputSerial().println(F("# in event handler : Application::getVersionInformation()"));
			#endif

			system.dump();
		}

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

				(this->*EVENT_HANDLER[header_id][cmd_id])();
			}
		}
	};

	bool (Application::*Application::CONTROLLER_EVENT_HANDLER[])() = {
		&Application::angleDiff,
		&Application::angle,
		&Application::homePosition,
		&Application::playMotion,
		&Application::stopMotion,
		&Application::playMotion,
		&Application::stopMotion
	};

	bool (Application::*Application::INTERPRETER_EVENT_HANDLER[])() = {
		&Application::popCode,
		&Application::pushCode,
		&Application::resetInterpreter
	};

	bool (Application::*Application::SETTER_EVENT_HANDLER[])() = {
		&Application::setHomeAngle,
		&Application::setMotionHeader, // sanity check.
		&Application::setJointSettings,
		&Application::setMaxAngle,
		&Application::setMotionFrame,
		&Application::setMotionHeader,
		&Application::setMinAngle
	};

	bool (Application::*Application::GETTER_EVENT_HANDLER[])() = {
		&Application::getJointSettings,
		&Application::getMotion,
		&Application::getVersionInformation
	};

	bool (Application::**Application::EVENT_HANDLER[])() = {
		Application::CONTROLLER_EVENT_HANDLER,
		Application::INTERPRETER_EVENT_HANDLER,
		Application::SETTER_EVENT_HANDLER,
		Application::GETTER_EVENT_HANDLER
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
