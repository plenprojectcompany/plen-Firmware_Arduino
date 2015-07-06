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


/*
namespace
{
	PLEN2::System system;
	
	PLEN2::ExternalEEPROM   external_eeprom;
	PLEN2::JointController  joint_controller;
	PLEN2::MotionController motion_player(joint_controller);
	PLEN2::Purser           purser;
}
*/


void PLEN2::setup()
{
	
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