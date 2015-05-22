// 標準Cライブラリ関連
#include <string.h>

// Arduinoライブラリ関連
#include <EEPROM.h>
#include <Wire.h>

// 独自ライブラリ関連
#include "Main.h"
#include "System.h"
#include "Pin.h"
#include "ExternalEEPROM.h"


#define _DEBUG           false //!< デバッグプリントを行います。
#define _DEBUG_INSTALL   false //!< モーションインストール回りのデバッグを行います。
#define _DEBUG_EXTEEPROM false //!< EEPROMの読み書き回りのデバッグを行います。
#define _DEBUG_HARD      false //!< 割り込み回りがシビアな処理についてもデバッグプリントを行います。


/*
namespace
{
	PLEN2::System system_ctrl;
	
	PLEN2::ExternalEEPROM   external_eeprom;
	PLEN2::JointController  joint_controller;
	PLEN2::MotionController motion_player(joint_controller);
	PLEN2::Purser           purser;
}
*/


/* TASK: ↓モーションクラスに統合する */
#define _PLEN2__SYSTEM__PWM_OUT_00_07_REGISTER OCR1C
#define _PLEN2__SYSTEM__PWM_OUT_08_15_REGISTER OCR1B
#define _PLEN2__SYSTEM__PWM_OUT_16_23_REGISTER OCR1A


void PLEN2::setup()
{
	pinMode(Pin::MULTIPLEXER_SELECT0(), OUTPUT);
	pinMode(Pin::MULTIPLEXER_SELECT1(), OUTPUT);
	pinMode(Pin::MULTIPLEXER_SELECT2(), OUTPUT);
	pinMode(Pin::PWM_OUT_00_07(),       OUTPUT);
	pinMode(Pin::PWM_OUT_08_15(),       OUTPUT);
	pinMode(Pin::PWM_OUT_16_23(),       OUTPUT);

	Joint::init();
	Motion::init();
}


void PLEN2::loop()
{
	if (Config::disable() && Motion::playing)
	{
		if (Motion::Frame::updatable())
		{
			Motion::Frame::_updatable = false;
			Motion::Frame::transition_count--;

			for (int index = 0; index < Joint::SUM(); index++)
			{
				Motion::Frame::now_fixed_point[index] += Motion::Frame::diff_fixed_point[index];
				Motion::Frame::now->joint_angle[index] = Motion::Frame::now_fixed_point[index] >> 16;
			}
		}

		if (Motion::Frame::updateFinished())
		{
			if (Motion::Frame::nextFrameLoadable())
			{
				Motion::Frame::buffering();
				Motion::Frame::transition_count = Motion::Frame::next->transition_delay_msec / Motion::Frame::UPDATE_MSEC();

				for (int index = 0; index < Joint::SUM(); index++)
				{
					Motion::Frame::now_fixed_point[index]  = (long)(Motion::Frame::now->joint_angle[index]) << 16;
					Motion::Frame::diff_fixed_point[index] = ((long)(Motion::Frame::next->joint_angle[index]) << 16) - Motion::Frame::now_fixed_point[index];
					Motion::Frame::diff_fixed_point[index] /= Motion::Frame::transition_count;
				}

				if (   (Motion::header.extra[0] == 1)
					&& (Motion::Frame::next->number == Motion::header.extra[2]))
				{
					Motion::getFrame(Motion::header.slot, Motion::header.extra[1], Motion::Frame::back);
				}
				else if (   (Motion::header.extra[0] == 2)
					     && (Motion::Frame::next->number == (Motion::header.frame_num - 1)))
				{
					Motion::getFrame(Motion::header.extra[1], 0, Motion::Frame::back);
				}
				else
				{
					Motion::getFrame(Motion::header.slot, Motion::Frame::next->number + 1, Motion::Frame::back);
				}

				for (int index = 0; index < Joint::SUM(); index++)
				{
					Motion::Frame::back->joint_angle[index] += Joint::SETTINGS[index].HOME;
				}
			}
			else
			{
				Motion::playing = false;
			}
		}
	}

	if (System::input_serial->available())
	{
		Purser::readByte(System::input_serial->read());

		if (Purser::lexAccept())
		{
			Purser::makeTokenLog();
			Purser::execEventHandler();
			Purser::transition();
		}
	}
}