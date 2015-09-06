#line 2 "MotionController.spec.ino"


// Arduinoライブラリ関連
#include <Wire.h>
#include <EEPROM.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "JointController.h"
#include "MotionController.h"

// テストケース選択用プリプロセスマクロ
#define _TEST_USER false //!< ユーザテストについても実行します。
#define _TEST_HARD false //!< プロセッサに負荷のかかるテストについても実行します。

// ファイル内グローバルインスタンスの定義
namespace
{
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
	expected.frame_num = random(PLEN2::MotionController::Header::FRAMENUM_MAX());
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
			expected.frame_num = random(PLEN2::MotionController::Header::FRAMENUM_MAX());
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
	@brief ランダムに選択したスロットへの、モーションフレームの設定テスト
*/
test(RandomSlotRandomFrame_SetFrame)
{
	const char SLOT = random(
		PLEN2::MotionController::Header::SLOT_MIN(),
		PLEN2::MotionController::Header::SLOT_MAX()
	);

	const char NUMBER = random(PLEN2::MotionController::Header::FRAMENUM_MAX());


	PLEN2::MotionController::Frame expected;
	expected.transition_time_ms = random();
	for (
		int index = 0;
		index < PLEN2::JointController::SUM();
		index++
	)
	{
		expected.joint_angle[index] = random();
	}
	expected.number = NUMBER;
	motion_ctrl.setFrame(SLOT, &expected);


	PLEN2::MotionController::Frame actual;
	actual.number = NUMBER;
	motion_ctrl.getFrame(SLOT, &actual);


	const char* p_expected = (const char*)&expected;
	const char* p_actual   = (const char*)&actual;
	for (
		int index = 0;
		index < sizeof(PLEN2::MotionController::Frame);
		index++
	)
	{
		assertEqual(p_expected[index], p_actual[index]);
	}
}

/*!
	@brief 全てのスロットへの、モーションフレームの設定テスト
*/
test(AllSlotRandomFrame_SetFrame)
{
	#if _TEST_HARD
		const char NUMBER = random(PLEN2::MotionController::Header::FRAMENUM_MAX());


		for (
			int slot = PLEN2::MotionController::Header::SLOT_MIN();
			slot < PLEN2::MotionController::Header::SLOT_MAX();
			slot++
		)
		{
			PLEN2::MotionController::Frame expected;
			expected.transition_time_ms = random();
			for (
				int index = 0;
				index < PLEN2::JointController::SUM();
				index++
			)
			{
				expected.joint_angle[index] = random();
			}
			expected.number = NUMBER;
			motion_ctrl.setFrame(slot, &expected);


			PLEN2::MotionController::Frame actual;
			actual.number =NUMBER;
			motion_ctrl.getFrame(slot, &actual);


			const char* p_expected = (const char*)&expected;
			const char* p_actual   = (const char*)&actual;
			for (
				int index = 0;
				index < sizeof(PLEN2::MotionController::Frame);
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
	@brief 未定義スロットへの、各種取得メソッドのテスト
*/
test(SlotOverflow_Methods)
{
	unsigned char SLOT = PLEN2::MotionController::Header::SLOT_MAX() + 1;
	PLEN2::MotionController::Header header;
	header.slot = SLOT;
	header.frame_num = 1;
	PLEN2::MotionController::Frame frame;
	frame.number = 0;

	bool expected = false;
	bool actual;


	actual = motion_ctrl.setHeader(&header);
	assertEqual(expected, actual);

	actual = motion_ctrl.getHeader(&header);
	assertEqual(expected, actual);

	actual = motion_ctrl.setFrame(SLOT, &frame);
	assertEqual(expected, actual);

	actual = motion_ctrl.getFrame(SLOT, &frame);
	assertEqual(expected, actual);
}

/*!
	@brief 未定義スロットへの、各種設定メソッドのテスト
*/
test(FrameNumOverflow_Methods)
{
	unsigned char FRAMENUM = PLEN2::MotionController::Header::FRAMENUM_MAX() + 1;
	PLEN2::MotionController::Header header;
	header.frame_num = FRAMENUM;
	header.slot = 0;
	PLEN2::MotionController::Frame frame;
	frame.number = FRAMENUM;

	bool expected = false;
	bool actual;


	actual = motion_ctrl.setHeader(&header);
	assertEqual(expected, actual);

	actual = motion_ctrl.setFrame(0, &frame);
	assertEqual(expected, actual);

	actual = motion_ctrl.getFrame(0, &frame);
	assertEqual(expected, actual);
}

/*!
	@brief 未定義スロットへの、各種設定メソッドのテスト
*/
test(FrameNumUnderflow_Methods)
{
	unsigned char FRAMENUM = PLEN2::MotionController::Header::FRAMENUM_MIN() - 1;
	PLEN2::MotionController::Header header;
	header.frame_num = FRAMENUM;
	header.slot = 0;

	bool expected = false;
	bool actual;


	actual = motion_ctrl.setHeader(&header);
	assertEqual(expected, actual);
}

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
	#if _TEST_USER && _TEST_HARD
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
