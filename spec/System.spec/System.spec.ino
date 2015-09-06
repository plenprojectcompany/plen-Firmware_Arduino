#line 2 "System.spec.ino"


// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "System.h"

// テストケース選択用プリプロセスマクロ
#define _TEST_USER true //!< ユーザテストについても実行します。

// ファイル内グローバルインスタンスの定義
namespace
{
	PLEN2::System system;
	PLEN2::System system_another;
}


/*!
	@brief System::toggleInputSerial()を1回実行するテスト

	当該メソッドの実行前後で、取得されるインスタンスが異なることを確認します。
*/
test(ToggleInputSerial)
{
	Stream* before = &(system.inputSerial());
	system.toggleInputSerial();
	Stream* after  = &(system.inputSerial());


	assertTrue(before != after);
}

/*!
	@brief System::toggleInputSerial()を2回実行するテスト

	当該メソッドの実行前後で、取得されるインスタンスが同一なことを確認します。
*/
test(ToggleInputSerial_x2)
{
	Stream* before = &(system.inputSerial());
	system.toggleInputSerial();
	system.toggleInputSerial();
	Stream* after  = &(system.inputSerial());


	assertTrue(before == after);
}

/*!
	@brief 入力シリアルインスタンスの同一性のテスト

	異なるSystemインスタンス間で、取得される入力シリアルインスタンスが
	同一なことを確認します。
*/
test(InputSerial_Identical)
{
	Stream* rhs = &(system.inputSerial());
	Stream* lhs = &(system_another.inputSerial());


	assertTrue(rhs == lhs);
}

/*!
	@brief 出力シリアルインスタンスの同一性のテスト

	異なるSystemインスタンス間で、取得される出力シリアルインスタンスが
	同一なことを確認します。
*/
test(OutputSerial_Identical)
{
	Stream* rhs = &(system.outputSerial());
	Stream* lhs = &(system_another.outputSerial());


	assertTrue(rhs == lhs);
}

/*!
	@brief システム構成のダンプテスト

	ユーザによる目視でのテストです。
*/
test(Dump)
{
	#if _TEST_USER
		system.dump();

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
	while (!Serial); // for the Arduino Leonardo/Micro only.
	
	Serial.println("Test started.");
	Serial.println("=============");
}

void loop()
{
	Test::run();
}
