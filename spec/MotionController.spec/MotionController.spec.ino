#line 2 "MotionController.spec.ino"


// Arduinoライブラリ関連
#include <Wire.h>
#include <EEPROM.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "JointController.h"
#include "MotionController.h"


#define _TEST_USER false //!< ユーザテストについても実行します。
#define _TEST_HARD false //!< プロセッサに負荷のかかるテストについても実行します。

namespace {
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
}


/*!
	@brief ランダムに選択したスロットへの、モーションヘッダの設定テスト
*/
test(RandomSlot_SetHeader)
{
	const char SLOT = random(
		PLEN2::MotionController::Header::SLOT_MIN(),
		PLEN2::MotionController::Header::SLOT_MAX()
	);

	PLEN2::MotionController::Header expected;
	expected.slot = SLOT;
	expected.frame_num = random();
	for (
		int index = 0;
		index < PLEN2::MotionController::Header::CODE_SIZE();
		index++
	)
	{
		expected.codes[index] = random();
	}
	for (
		int index = 0;
		index < PLEN2::MotionController::Header::NAME_LENGTH();
		index++
	)
	{
		expected.name[index] = random();
	}
	motion_ctrl.setHeader(&expected);


	PLEN2::MotionController::Header actual;
	actual.slot = SLOT;
	motion_ctrl.getHeader(&actual);


	const char* p_expected = (const char*)&expected;
	const char* p_actual   = (const char*)&actual;
	for (
		int index = 0;
		index < sizeof(PLEN2::MotionController::Header);
		index++
	)
	{
		assertEqual(p_expected[index], p_actual[index]);
	}
}

/*!
	@brief 全てのスロットへの、モーションヘッダの設定テスト
*/
test(AllSlot_SetHeader)
{
	#if _TEST_HARD
		for (
			int slot = PLEN2::MotionController::Header::SLOT_MIN();
			slot < PLEN2::MotionController::Header::SLOT_MAX();
			slot++
		)
		{
			PLEN2::MotionController::Header expected;
			expected.slot = slot;
			expected.frame_num = random();
			for (
				int index = 0;
				index < PLEN2::MotionController::Header::CODE_SIZE();
				index++
			)
			{
				expected.codes[index] = random();
			}
			for (
				int index = 0;
				index < PLEN2::MotionController::Header::NAME_LENGTH();
				index++
			)
			{
				expected.name[index] = random();
			}
			motion_ctrl.setHeader(&expected);


			PLEN2::MotionController::Header actual;
			actual.slot = slot;
			motion_ctrl.getHeader(&actual);


			const char* p_expected = (const char*)&expected;
			const char* p_actual   = (const char*)&actual;
			for (
				int index = 0;
				index < sizeof(PLEN2::MotionController::Header);
				index++
			)
			{
				assertEqual(p_expected[index], p_actual[index]);
			}
		}
	#else
		skip();
	#endif
}

/*!
	@brief 
*/

/*!
	@brief ランダムに選択したスロットのダンプテスト

	ユーザによる目視でのテストです。
*/
test(RandomSlot_Dump)
{
	#if _TEST_USER
		const char SLOT = random(
			PLEN2::MotionController::Header::SLOT_MIN(),
			PLEN2::MotionController::Header::SLOT_MAX()
		);

		motion_ctrl.dump(SLOT);

		pass();
	#else
		skip();
	#endif
}

/*!
	@brief 全てのスロットのダンプテスト

	ユーザによる目視でのテストです。
*/
test(AllSlot_Dump)
{
	#if _TEST_USER
		for (
			int slot = PLEN2::MotionController::Header::SLOT_MIN();
			slot < PLEN2::MotionController::Header::SLOT_MAX();
			slot++
		)
		{
			motion_ctrl.dump(slot);
		}

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
	randomSeed(analogRead(0));
	joint_ctrl.loadSettings();
	joint_ctrl.resetSettings();

	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.println("Test started.");
	Serial.println("=============");
}

void loop()
{
	Test::run();
}
