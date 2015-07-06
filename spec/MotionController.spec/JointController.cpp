// Arduinoライブラリ関連
#include "Arduino.h"
#include <EEPROM.h>

// 独自ライブラリ関連
#include "Pin.h"
#include "System.h"
#include "JointController.h"


// マクロの定義
#define _DEBUG      false
#define _DEBUG_HARD false

#define _PLEN2__JOINTCONTROLLER__PWM_OUT_00_07_REGISTER OCR1C
#define _PLEN2__JOINTCONTROLLER__PWM_OUT_08_15_REGISTER OCR1B
#define _PLEN2__JOINTCONTROLLER__PWM_OUT_16_23_REGISTER OCR1A


// ファイル内グローバルインスタンスの定義
namespace {
	PLEN2::System system;
}


volatile unsigned char PLEN2::JointController::_overflow_count;
unsigned int PLEN2::JointController::_pwms[_PLEN2__JOINTCONTROLLER__SUM];


PLEN2::JointController::JointController()
{
	pinMode(PLEN2::Pin::MULTIPLEXER_SELECT0(), OUTPUT);
	pinMode(PLEN2::Pin::MULTIPLEXER_SELECT1(), OUTPUT);
	pinMode(PLEN2::Pin::MULTIPLEXER_SELECT2(), OUTPUT);
	pinMode(PLEN2::Pin::PWM_OUT_00_07(),       OUTPUT);
	pinMode(PLEN2::Pin::PWM_OUT_08_15(),       OUTPUT);
	pinMode(PLEN2::Pin::PWM_OUT_16_23(),       OUTPUT);

	_SETTINGS_INITIAL[0]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 900 ); // [01] 左：肩ピッチ
	_SETTINGS_INITIAL[1]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1050); // [02] 左：腿ヨー
	_SETTINGS_INITIAL[2]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1250); // [03] 左：肩ロール
	_SETTINGS_INITIAL[3]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 800 ); // [04] 左：肘ロール
	_SETTINGS_INITIAL[4]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 800 ); // [05] 左：腿ロール
	_SETTINGS_INITIAL[5]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 850 ); // [06] 左：腿ピッチ
	_SETTINGS_INITIAL[6]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1400); // [07] 左：膝ピッチ
	_SETTINGS_INITIAL[7]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1200); // [08] 左：足首ピッチ
	_SETTINGS_INITIAL[8]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 850 ); // [09] 左：足首ロール
	_SETTINGS_INITIAL[12] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 900 ); // [10] 右：肩ピッチ
	_SETTINGS_INITIAL[13] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 950 ); // [11] 右：腿ヨー
	_SETTINGS_INITIAL[14] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 550 ); // [12] 右：肩ロール
	_SETTINGS_INITIAL[15] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1000); // [13] 右：肘ロール
	_SETTINGS_INITIAL[16] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1000); // [14] 右：腿ロール
	_SETTINGS_INITIAL[17] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1050); // [15] 右：腿ピッチ
	_SETTINGS_INITIAL[18] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 400 ); // [16] 右：膝ピッチ
	_SETTINGS_INITIAL[19] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 600 ); // [17] 右：足首ピッチ
	_SETTINGS_INITIAL[20] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 950 ); // [18] 右：足首ロール

	for (int joint_id = 0; joint_id < SUM(); joint_id++)
	{
		_SETTINGS[joint_id] = _SETTINGS_INITIAL[joint_id];
		setAngle(joint_id, _SETTINGS[joint_id].HOME);
	}
}


void PLEN2::JointController::loadSettings()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::loadSettings()"));
	#endif

	unsigned char* filler = (unsigned char*)_SETTINGS;
	
	if (EEPROM.read(FLAG_ADDRESS()) != FLAG_VALUE())
	{
		EEPROM.write(FLAG_ADDRESS(), FLAG_VALUE());
		delay(5);

		for (int index = 0; index < sizeof(_SETTINGS); index++)
		{
			EEPROM.write(SETTINGS_BEGIN_ADDRESS() + index, filler[index]);
			delay(5);
		}
	}
	else
	{
		for (int index = 0; index < sizeof(_SETTINGS); index++)
		{
			filler[index] = EEPROM.read(SETTINGS_BEGIN_ADDRESS() + index);
		}
	}

	for (int joint_id = 0; joint_id < SUM(); joint_id++)
	{
		setAngle(joint_id, _SETTINGS[joint_id].HOME);
	}

	/*
		@brief タイマ1の設定

		@attention
		比較一致の出力はLOWレベルの方が直感的でわかりやすいかと思いますが、
		それだとマルチプレクサで出力先を切り替える処理の最中にもPWM信号が
		出力されてしまうため、出力先切り替え時にインパルスノイズが乗ります。
	*/
	cli();

	TCCR1A =
		_BV(WGM11)  | _BV(WGM10)  | // 10bit高速PWM動作に設定
		_BV(COM1A1) | _BV(COM1A0) | // 比較一致でOC1AをHIGHレベルに出力
		_BV(COM1B1) | _BV(COM1B0) | // 比較一致でOC1BをHIGHレベルに出力
		_BV(COM1C1) | _BV(COM1C0);  // 比較一致でOC1CをHIGHレベルに出力

	TCCR1B = 
		_BV(WGM12) |                // 10bit高速PWM動作に設定
		_BV(CS11)  | _BV(CS10);     // 前置分周64に設定

	TIFR1 = _BV(OCF1A) | _BV(OCF1B) | _BV(OCF1C) | _BV(TOV1); // 割り込みフラグをクリア

	sei();
	
	TIMSK1 = _BV(TOIE1);
}


void PLEN2::JointController::resetSettings()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::resetSettings()"));
	#endif

	unsigned char* filler = (unsigned char*)_SETTINGS_INITIAL;

	EEPROM.write(FLAG_ADDRESS(), FLAG_VALUE());
	delay(5);

	for (int index = 0; index < sizeof(_SETTINGS_INITIAL); index++)
	{
		EEPROM.write(SETTINGS_BEGIN_ADDRESS() + index, filler[index]);
		delay(5);
	}

	for (int joint_id = 0; joint_id < SUM(); joint_id++)
	{
		_SETTINGS[joint_id] = _SETTINGS_INITIAL[joint_id];
		setAngle(joint_id, _SETTINGS[joint_id].HOME);
	}
}


int PLEN2::JointController::getMinAngle(unsigned char joint_id)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::getMinAngle()"));
	#endif

	if (joint_id >= SUM())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> joint_id : bad argment (value : "));
			system.outputSerial().print((int)joint_id);
			system.outputSerial().println(F(")"));
		#endif

		return -1;
	}

	return _SETTINGS[joint_id].MIN;
}


int PLEN2::JointController::getMaxAngle(unsigned char joint_id)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::getMaxAngle()"));
	#endif

	if (joint_id >= SUM())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> joint_id : bad argment (value : "));
			system.outputSerial().print((int)joint_id);
			system.outputSerial().println(F(")"));
		#endif

		return -1;
	}

	return _SETTINGS[joint_id].MAX;
}


int PLEN2::JointController::getHomeAngle(unsigned char joint_id)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::getHomeAngle()"));
	#endif

	if (joint_id >= SUM())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> joint_id : bad argment (value : "));
			system.outputSerial().print((int)joint_id);
			system.outputSerial().println(F(")"));
		#endif

		return -1;
	}

	return _SETTINGS[joint_id].HOME;
}


bool PLEN2::JointController::setMinAngle(unsigned char joint_id, unsigned int angle)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::setMinAngle()"));
	#endif

	if (joint_id >= SUM())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> joint_id : bad argment (value : "));
			system.outputSerial().print((int)joint_id);
			system.outputSerial().println(F(")"));
		#endif

		return false;
	}

	_SETTINGS[joint_id].MIN = angle;

	unsigned char* filler = (unsigned char*)&(_SETTINGS[joint_id].MIN);
	int address_offset = (int)filler - (int)_SETTINGS;

	for (int index = 0; index < sizeof(_SETTINGS[joint_id].MIN); index++)
	{
		EEPROM.write(SETTINGS_BEGIN_ADDRESS() + address_offset + index, filler[index]);
		delay(5);
	}

	#if _DEBUG
		system.outputSerial().print(F(">>> address_offset : "));
		system.outputSerial().println(address_offset);
	#endif

	return true;
}


bool PLEN2::JointController::setMaxAngle(unsigned char joint_id, unsigned int angle)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::setMaxAngle()"));
	#endif

	if (joint_id >= SUM())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> joint_id : bad argment (value : "));
			system.outputSerial().print((int)joint_id);
			system.outputSerial().println(F(")"));
		#endif

		return false;
	}

	_SETTINGS[joint_id].MAX = angle;

	unsigned char* filler = (unsigned char*)&(_SETTINGS[joint_id].MAX);
	int address_offset = (int)filler - (int)_SETTINGS;

	for (int index = 0; index < sizeof(_SETTINGS[joint_id].MAX); index++)
	{
		EEPROM.write(SETTINGS_BEGIN_ADDRESS() + address_offset + index, filler[index]);
		delay(5);
	}

	#if _DEBUG
		system.outputSerial().print(F(">>> address_offset : "));
		system.outputSerial().println(address_offset);
	#endif

	return true;
}


bool PLEN2::JointController::setHomeAngle(unsigned char joint_id, unsigned int angle)
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::setHomeAngle()"));
	#endif

	if (joint_id >= SUM())
	{
		#if _DEBUG
			system.outputSerial().print(F(">>> joint_id : bad argment (value : "));
			system.outputSerial().print((int)joint_id);
			system.outputSerial().println(F(")"));
		#endif

		return false;
	}

	_SETTINGS[joint_id].HOME = angle;

	unsigned char* filler = (unsigned char*)&(_SETTINGS[joint_id].HOME);
	int address_offset = (int)filler - (int)_SETTINGS;

	for (int index = 0; index < sizeof(_SETTINGS[joint_id].HOME); index++)
	{
		EEPROM.write(SETTINGS_BEGIN_ADDRESS() + address_offset + index, filler[index]);
		delay(5);
	}

	#if _DEBUG
		system.outputSerial().print(F(">>> address_offset : "));
		system.outputSerial().println(address_offset);
	#endif

	return true;
}


bool PLEN2::JointController::setAngle(unsigned char joint_id, unsigned int angle)
{
	#if _DEBUG_HARD
		system.outputSerial().println(F("in fuction : JointController::setAngle()"));
	#endif

	if (joint_id >= SUM())
	{
		#if _DEBUG_HARD
			system.outputSerial().print(F(">>> joint_id : bad argment (value : "));
			system.outputSerial().print((int)joint_id);
			system.outputSerial().println(F(")"));
		#endif

		return false;
	}

	angle = constrain(angle, _SETTINGS[joint_id].MIN, _SETTINGS[joint_id].MAX);

	/*!
		@attention
		反転出力モードに設定しているため、PWM_MAX() < PWM_MIN()の順序が正しいです。
	*/
	_pwms[joint_id] = constrain(
		map(
			angle,
			PLEN2::JointController::ANGLE_MIN(), PLEN2::JointController::ANGLE_MAX(),
			PLEN2::JointController::PWM_MIN(),   PLEN2::JointController::PWM_MAX()
		),
		PLEN2::JointController::PWM_MAX(), PLEN2::JointController::PWM_MIN()
	);

	return true;
}


void PLEN2::JointController::dump()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : JointController::dump()"));
	#endif

	system.outputSerial().println(F("{"));
	system.outputSerial().println(F("\t\"settings\" : ["));
	for (int joint_id = 0; joint_id < SUM(); joint_id++)
	{
		system.outputSerial().println(F("\t\t{"));
		system.outputSerial().print(F("\t\t\t\"joint\" : "));
		system.outputSerial().print(joint_id);
		system.outputSerial().println(F(","));
		system.outputSerial().print(F("\t\t\t\"MAX\"   : "));
		system.outputSerial().print(_SETTINGS[joint_id].MAX);
		system.outputSerial().println(F(","));
		system.outputSerial().print(F("\t\t\t\"MIN\"   : "));
		system.outputSerial().print(_SETTINGS[joint_id].MIN);
		system.outputSerial().println(F(","));
		system.outputSerial().print(F("\t\t\t\"HOME\"  : "));
		system.outputSerial().println(_SETTINGS[joint_id].HOME);
		system.outputSerial().print(F("\t\t}"));
		system.outputSerial().println(F(","));
	}
	system.outputSerial().println(F("\t]"));
	system.outputSerial().println(F("}"));
}


/*!
	@brief タイマ1 オーバーフロー割り込みベクタ

	@note
	実行タイミングはTCNT1がオーバーフローしたときです。
	PLEN2の場合では、16[MHz]に64分周をかけ、かつ10bitのカウンタを使用するので、
	(16,000,000 / (64 * 1,024))^-1 * 1,000 = 4.096[msec]ごとに割り込まれます。
	<br><br>
	この4.096[msec]という値は、サーボモータのPWM信号許容入力間隔に対して
	十分に小さな値です。そこで、各サーボモータに対して割り込み8回に1回の割合で
	PWM信号を入力し、割り込み毎に出力先のサーボモータを切り替えることで、
	複数のサーボモータの制御を実現しています。

	@attention
	内部で変更される変数は、基本的にvolatile修飾子をつけるのが無難です。
	コンパイラでの最適化による、わかりづらいバグを防ぐことができます。
	<br><br>
	AVRマイコンではPWM信号の出力がダブルバッファリングによって制御されるため、
	PWM信号の出力値を関節値の参照先より1つ先読みする必要があります。
	この操作を怠った場合、複数のサーボモータ制御は意図しない挙動をするので、
	十分に注意してください。
*/
ISR(TIMER1_OVF_vect)
{
	#if _DEBUG_HARD
		system.outputSerial().println(F("in vector : TIMER1_OVF_vect"));
	#endif

	volatile static unsigned char output_select = 0;
	volatile static unsigned char joint_select  = 1; // @attention ダブルバッファリングを考慮して1つ先読み

	/*!
		@attention
		PWM信号が出力される前に出力先を切り替える必要があるので、
		タイマ割り込みのなるべく早い段階で切り替え処理を行う。

		@todo
		- PORTレジスタの直接操作による高速化
	*/
	digitalWrite(PLEN2::Pin::MULTIPLEXER_SELECT0(), bitRead(output_select, 0));
	digitalWrite(PLEN2::Pin::MULTIPLEXER_SELECT1(), bitRead(output_select, 1));
	digitalWrite(PLEN2::Pin::MULTIPLEXER_SELECT2(), bitRead(output_select, 2));

	_PLEN2__JOINTCONTROLLER__PWM_OUT_00_07_REGISTER = PLEN2::JointController::_pwms[
		joint_select + 0 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()
	];
	
	_PLEN2__JOINTCONTROLLER__PWM_OUT_08_15_REGISTER = PLEN2::JointController::_pwms[
		joint_select + 1 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()
	];
	
	_PLEN2__JOINTCONTROLLER__PWM_OUT_16_23_REGISTER = PLEN2::JointController::_pwms[
		joint_select + 2 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()
	];

	++output_select &= (PLEN2::JointController::Multiplexer::SELECTABLE_NUM() - 1);
	++joint_select  &= (PLEN2::JointController::Multiplexer::SELECTABLE_NUM() - 1);

	PLEN2::JointController::_overflow_count++;
}
