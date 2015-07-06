#line 2 "JointController.spec.ino"

// Arduinoライブラリ関連
#include <EEPROM.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "JointController.h"


#define _TEST_USER false //!< ユーザテストについても実行します。

namespace {
	PLEN2::JointController joint_ctrl;
}


/*!
	@brief テストケースの定義
*/
test(LoadSettings)
{
	pass();
}

/*!
	@brief 
*/
test(ResetSettings)
{
	pass();
}

test(SetMinAngle)
{
	pass();
}

test(SetMaxAngle)
{
	pass();
}

test(SetHomeAngle)
{
	pass();
}

test(SetAngle)
{
	pass();
}

/*!
	@brief 
*/
test(Timer1Attached)
{
	unsigned char before = joint_ctrl._overflow_count;	
	while (before == joint_ctrl._overflow_count);

	
	pass();
}

/*!
	@brief 関節設定のダンプテスト

	ユーザによる目視でのテストです。
*/
test(Dump)
{
	#if _TEST_USER
		joint_ctrl.dump();

		pass();
	#else
		skip();
	#endif
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	while(!Serial); // for the Arduino Leonardo/Micro only.

	Serial.println("Test started.");
	Serial.println("=============");
}

void loop()
{
	Test::run();
}
