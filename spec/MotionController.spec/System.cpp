// 独自ライブラリ関連
#include "Pin.h"
#include "System.h"


// マクロの定義
#define _DEBUG false

#define _PLEN2__SYSTEM__USBSERIAL Serial
#define _PLEN2__SYSTEM__BLESERIAL Serial1


Stream* PLEN2::System::_p_input_serial;
Stream* PLEN2::System::_p_output_serial;


PLEN2::System::System()
{
	_p_input_serial  = &_PLEN2__SYSTEM__BLESERIAL;
	_p_output_serial = &_PLEN2__SYSTEM__USBSERIAL;

	_PLEN2__SYSTEM__BLESERIAL.begin(BLESERIAL_BAUDRATE());
	_PLEN2__SYSTEM__USBSERIAL.begin(USBSERIAL_BAUDRATE());

	pinMode(Pin::RS485_TXD(), OUTPUT);
	pinMode(Pin::LED_OUT(),   OUTPUT);

	digitalWrite(Pin::RS485_TXD(), LOW);
	digitalWrite(Pin::LED_OUT(),   LOW);
}


Stream& PLEN2::System::inputSerial()
{
	return *_p_input_serial;
}


Stream& PLEN2::System::outputSerial()
{
	return *_p_output_serial;
}


void PLEN2::System::toggleInputSerial()
{
	#if _DEBUG
		outputSerial().println(F("in fuction : System::toggleInputSerial()"));
	#endif

	if (_p_input_serial == &_PLEN2__SYSTEM__BLESERIAL)
	{
		_p_input_serial = &_PLEN2__SYSTEM__USBSERIAL;
	}
	else
	{
		_p_input_serial = &_PLEN2__SYSTEM__BLESERIAL;
	}
}


void PLEN2::System::timer1Attach()
{
	#if _DEBUG
		outputSerial().println(F("in fuction : System::timer1Attach()"));
	#endif

	TIMSK1 = _BV(TOIE1);
}


void PLEN2::System::timer1Detach()
{
	#if _DEBUG
		outputSerial().println(F("in fuction : System::timer1Detach()"));
	#endif

	TIMSK1 &= ~_BV(TOIE1);
}
