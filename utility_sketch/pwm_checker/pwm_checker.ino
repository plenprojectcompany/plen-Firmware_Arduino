#include <EEPROM.h>
#include "System.h"
#include "JointController.h"


namespace {
	PLEN2::System          system;
	PLEN2::JointController joint_ctrl;

	unsigned int pwm = 512;
}


void setup()
{
	joint_ctrl.loadSettings();
}

void loop()
{
	if (system.USBSerial().available())
	{
		switch (system.USBSerial().read())
		{
			case 'p':
			{
				if (pwm != 1023)
				{
					pwm++;
				}

				break;
			}

			case 'm':
			{
				if (pwm != 0)
				{
					pwm--;
				}

				break;
			}

			default:
			{
				// noop.

				break;
			}
		}

		system.USBSerial().print(F("output : "));
		system.USBSerial().println(pwm);

		joint_ctrl._pwms[0] = pwm;
	}
}
