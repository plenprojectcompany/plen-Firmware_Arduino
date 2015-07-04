#line 2 "System.spec.ino"

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "System.h"


namespace {
	PLEN2::System system;
}


/*!
	@brief テストケースの定義
*/
test(ToggleInputSerial)
{
	Stream* before = &(system.inputSerial());
	system.toggleInputSerial();
	Stream* after  = &(system.inputSerial());


	assertTrue(before != after);
}

test(ToggleInputSerial_x2)
{
	Stream* before = &(system.inputSerial());
	system.toggleInputSerial();
	system.toggleInputSerial();
	Stream* after  = &(system.inputSerial());


	assertTrue(before == after);
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	while(!Serial); // for the Arduino Leonardo/Micro only.
}

void loop()
{
	Test::run();
}
