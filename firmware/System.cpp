/*
	Copyright (c) 2015,
	- Kazuyuki TAKASE - https://github.com/Guvalif
	- PLEN Project Company Ltd. - http://plen.jp

	This software is released under the MIT License.
	(See also : http://opensource.org/licenses/mit-license.php)
*/


#include "Arduino.h"
#include "Pin.h"
#include "System.h"


// マクロ定義
#define DEBUG false

#define PLEN2_SYSTEM_USBSERIAL Serial
#define PLEN2_SYSTEM_BLESERIAL Serial1


PLEN2::System::System()
{
	PLEN2_SYSTEM_BLESERIAL.begin(BLESERIAL_BAUDRATE());
	PLEN2_SYSTEM_USBSERIAL.begin(USBSERIAL_BAUDRATE());

	pinMode(Pin::RS485_TXD(), OUTPUT);
	pinMode(Pin::LED_OUT(),   OUTPUT);

	digitalWrite(Pin::RS485_TXD(), LOW);
	digitalWrite(Pin::LED_OUT(),   LOW);
}


Stream& PLEN2::System::USBSerial()
{
	return PLEN2_SYSTEM_USBSERIAL;
}


Stream& PLEN2::System::BLESerial()
{
	return PLEN2_SYSTEM_BLESERIAL;
}


Stream& PLEN2::System::inputSerial()
{
	return PLEN2_SYSTEM_BLESERIAL;
}


Stream& PLEN2::System::outputSerial()
{
	return PLEN2_SYSTEM_USBSERIAL;
}


void PLEN2::System::timer1Attach()
{
	#if DEBUG
		outputSerial().println(F("=== running in function : System::timer1Attach()"));
	#endif

	TIMSK1 = _BV(TOIE1);
}


void PLEN2::System::timer1Detach()
{
	#if DEBUG
		outputSerial().println(F("=== running in function : System::timer1Detach()"));
	#endif

	TIMSK1 &= ~_BV(TOIE1);
}


void PLEN2::System::dump()
{
	#if DEBUG
		outputSerial().println(F("=== running in function : System::dump()"));
	#endif

	outputSerial().println(F("{"));
		outputSerial().print(F("\t\"device\": \""));
		outputSerial().print(DEVICE());
		outputSerial().println(F("\","));

		outputSerial().print(F("\t\"codename\": \""));
		outputSerial().print(CODENAME());
		outputSerial().println(F("\","));

		outputSerial().print(F("\t\"version\": \""));
		outputSerial().print(VERSION());
		outputSerial().println(F("\""));
	outputSerial().println(F("}"));
}
