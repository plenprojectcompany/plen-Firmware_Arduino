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
	#if _DEBUG || _DEBUG_HARD
		PLEN2::System system;
	#endif
}


volatile unsigned char PLEN2::JointController::_overflow_count;
volatile unsigned int  PLEN2::JointController::_pwms[_PLEN2__JOINTCONTROLLER__SUM];


void PLEN2::JointController::loadSettings()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : Joint::init()"));
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
		for (int index = 0; index < sizeof(SETTINGS); index++)
		{
			filler[index] = EEPROM.read(SETTINGS_BEGIN_ADDRESS() + index);
		}
	}
}


PLEN2::JointController::JointController()
{
	#if _DEBUG
		system.outputSerial().println(F("in fuction : Joint::constructor()"));
	#endif

	pinMode(Pin::MULTIPLEXER_SELECT0(), OUTPUT);
	pinMode(Pin::MULTIPLEXER_SELECT1(), OUTPUT);
	pinMode(Pin::MULTIPLEXER_SELECT2(), OUTPUT);
	pinMode(Pin::PWM_OUT_00_07(),       OUTPUT);
	pinMode(Pin::PWM_OUT_08_15(),       OUTPUT);
	pinMode(Pin::PWM_OUT_16_23(),       OUTPUT);

	_SETTINGS[0]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 900 ); // [01] 左：肩ピッチ
	_SETTINGS[1]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1050); // [02] 左：腿ヨー
	_SETTINGS[2]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1250); // [03] 左：肩ロール
	_SETTINGS[3]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 800 ); // [04] 左：肘ロール
	_SETTINGS[4]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 800 ); // [05] 左：腿ロール
	_SETTINGS[5]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 850 ); // [06] 左：腿ピッチ
	_SETTINGS[6]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1400); // [07] 左：膝ピッチ
	_SETTINGS[7]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1200); // [08] 左：足首ピッチ
	_SETTINGS[8]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 850 ); // [09] 左：足首ロール
	_SETTINGS[9]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[10] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[11] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[12] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 900 ); // [10] 右：肩ピッチ
	_SETTINGS[13] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 950 ); // [11] 右：腿ヨー
	_SETTINGS[14] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 550 ); // [12] 右：肩ロール
	_SETTINGS[15] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1000); // [13] 右：肘ロール
	_SETTINGS[16] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1000); // [14] 右：腿ロール
	_SETTINGS[17] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1050); // [15] 右：腿ピッチ
	_SETTINGS[18] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 400 ); // [16] 右：膝ピッチ
	_SETTINGS[19] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 600 ); // [17] 右：足首ピッチ
	_SETTINGS[20] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 950 ); // [18] 右：足首ロール
	_SETTINGS[21] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[22] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[23] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
}


	// _PLEN2__JOINTCONTROLLER__PWM_OUT_00_07_REGISTER = constrain(
	// 	map(
	// 		Motion::Frame::now->joint_angle[joint_select + 0 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()],
	// 		PLEN2::JointController::ANGLE_MIN(), PLEN2::JointController::ANGLE_MAX(),
	// 		PLEN2::JointController::PWM::MIN(),  PLEN2::JointController::PWM::MAX()
	// 	),
	// 	PLEN2::JointController::PWM::MAX(), PLEN2::JointController::PWM::MIN()
	// );

	// _PLEN2__JOINTCONTROLLER__PWM_OUT_08_15_REGISTER = constrain(
	// 	map(
	// 		Motion::Frame::now->joint_angle[joint_select + 1 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()],
	// 		PLEN2::JointController::ANGLE_MIN(), PLEN2::JointController::ANGLE_MAX(),
	// 		PLEN2::JointController::PWM::MIN(),  PLEN2::JointController::PWM::MAX()
	// 	),
	// 	PLEN2::JointController::PWM::MAX(), PLEN2::JointController::PWM::MIN()
	// );

	// _PLEN2__JOINTCONTROLLER__PWM_OUT_16_23_REGISTER = constrain(
	// 	map(
	// 		Motion::Frame::now->joint_angle[joint_select + 2 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()],
	// 		PLEN2::JointController::ANGLE_MIN(), PLEN2::JointController::ANGLE_MAX(),
	// 		PLEN2::JointController::PWM::MIN(),  PLEN2::JointController::PWM::MAX()
	// 	),
	// 	PLEN2::JointController::PWM::MAX(), PLEN2::JointController::PWM::MIN()
	// );


/*!
	@brief タイマ1 オーバーフロー割り込みベクタ

	@note
	実行タイミングはTCNT1がオーバーフローしたときです。
	PLEN2の場合では、16[MHz]に64分周をかけ、かつ10bitのカウンタを使用するので、
	(16,000,000 / (64 * 1,024))^-1 * 1,000 = 4.096[msec]ごとに割り込まれます。
	<br>
	この4.096[msec]という値は、サーボモータのPWM信号許容入力間隔に対して
	十分に小さな値です。そこで、各サーボモータに対して割り込み8回に1回の割合で
	PWM信号を入力し、割り込み毎に出力先のサーボモータを切り替えることで、
	複数のサーボモータの制御を実現しています。

	@attention
	内部で変更される変数は、基本的にvolatile修飾子をつけるのが無難です。
	コンパイラでの最適化による、わかりづらいバグを防ぐことができます。
	<br>
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
	*/
	digitalWrite(Pin::MULTIPLEXER_SELECT0(), bitRead(output_select, 0));
	digitalWrite(Pin::MULTIPLEXER_SELECT1(), bitRead(output_select, 1));
	digitalWrite(Pin::MULTIPLEXER_SELECT2(), bitRead(output_select, 2));

	_PLEN2__JOINTCONTROLLER__PWM_OUT_00_07_REGISTER = PLEN2::JointController::_pwms[
		joint_select + 0 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()
	];
	
	_PLEN2__JOINTCONTROLLER__PWM_OUT_08_15_REGISTER = PLEN2::JointController::_pwms[
		joint_select + 1 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()
	];
	
	_PLEN2__JOINTCONTROLLER__PWM_OUT_16_23_REGISTER = PLEN2::JointController::_pwms[
		joint_select + 2 * PLEN2::JointController::Multiplexer::SELECTABLE_NUM()
	];

	output_select = (++output_select) & (PLEN2::JointController::Multiplexer::SELECTABLE_NUM() - 1);
	joint_select  = (++joint_select)  & (PLEN2::JointController::Multiplexer::SELECTABLE_NUM() - 1);

	PLEN2::JointController::_overflow_count++;
}