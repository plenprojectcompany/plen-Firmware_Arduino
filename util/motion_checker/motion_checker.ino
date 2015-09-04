#include <stdlib.h>
#include <Wire.h>
#include <EEPROM.h>
#include "System.h"
#include "JointController.h"
#include "MotionController.h"


namespace
{
	PLEN2::System           system;
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
}

namespace Purser
{
	char buffer[4] = { '\0', '\0', '\0', '\0' };
	char position  = 0;
}


void setup()
{
	joint_ctrl.loadSettings();
	joint_ctrl.resetSettings();
}

void loop()
{
loop_begin:

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
		}
	}

	if (system.USBSerial().available())
	{
		if (Purser::position == 0)
		{
			if (system.USBSerial().peek() != '$')
			{
				system.USBSerial().read();

				goto loop_begin;
			}

			system.USBSerial().read();
			Purser::position++;

			goto loop_begin;
		}

		Purser::buffer[Purser::position++] = system.USBSerial().read();

		if (Purser::position == 3)
		{
			motion_ctrl.dump(atoi(Purser::buffer + 1));
			motion_ctrl.play(atoi(Purser::buffer + 1));
			
			Purser::position = 0;
		}
	}
}
