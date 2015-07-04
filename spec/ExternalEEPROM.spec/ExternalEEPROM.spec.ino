#line 2 "ExternalEEPROM.spec.ino"

// Arduinoライブラリ関連
#include <Wire.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "ExternalEEPROM.h"


#define _TEST_HARD false //!< プロセッサに負荷のかかるテストについても実行します。

namespace {
	PLEN2::ExternalEEPROM ext_eeprom;
}


/*!
	@brief テストケースの定義
*/
test(RandomSlot_ReadWrite) 
{
	#define _RANDOMSLOT_READWRITE__BUFFER_SIZE 30
	const int BUFFER_SIZE = 30;

	const int slot = random(PLEN2::ExternalEEPROM::SLOT_MIN(), PLEN2::ExternalEEPROM::SLOT_MAX());


	char expected[_RANDOMSLOT_READWRITE__BUFFER_SIZE] = { 0 };
	for (int index = 0; index < BUFFER_SIZE; index++)
	{
		expected[index] = random(-128, 128);
	}
	ext_eeprom.writeSlot(slot, expected, BUFFER_SIZE);


	char actual[_RANDOMSLOT_READWRITE__BUFFER_SIZE] = { 0 };
	ext_eeprom.readSlot(slot, actual, BUFFER_SIZE);


	for (int index = 0; index < BUFFER_SIZE; index++)
	{
		assertEqual(expected[index], actual[index]);
	}
}

/*!
	@attention
	このテストの実行には約41秒の時間を要します。
	また、EEPROMの寿命を著しく縮めるため、普段は実行しないことをおすすめします。
*/
test(AllSlot_ReadWrite)
{
	#if _TEST_HARD
		#define _ALLSLOT_READWRITE__BUFFER_SIZE 30
		const int BUFFER_SIZE = 30;


		for (int slot = PLEN2::ExternalEEPROM::SLOT_MIN(); slot < PLEN2::ExternalEEPROM::SLOT_MAX(); slot++)
		{
			char expected[_ALLSLOT_READWRITE__BUFFER_SIZE] = { 0 };
			for (int index = 0; index < BUFFER_SIZE; index++)
			{
				expected[index] = random(-128, 128);
			}
			ext_eeprom.writeSlot(slot, expected, BUFFER_SIZE);


			char actual[_ALLSLOT_READWRITE__BUFFER_SIZE] = { 0 };
			ext_eeprom.readSlot(slot, actual, BUFFER_SIZE);


			for (int index = 0; index < BUFFER_SIZE; index++)
			{
				assertEqual(expected[index], actual[index]);
			}
		}
	#else
		skip();
	#endif
}

test(RandomSlot_ReadOverflow)
{
	#define _RANDOMSLOT_READOVERFLOW__BUFFER_SIZE 33
	const int BUFFER_SIZE = 33;

	const int slot = random(PLEN2::ExternalEEPROM::SLOT_MIN(), PLEN2::ExternalEEPROM::SLOT_MAX());
	char data[_RANDOMSLOT_READOVERFLOW__BUFFER_SIZE] = { 0 };


	int expected = -1;
	int actual   = ext_eeprom.readSlot(slot, data, BUFFER_SIZE);


	assertEqual(expected, actual);
}

test(RandomSlot_WriteOverflow)
{
	#define _RANDOMSLOT_WRITEOVERFLOW__BUFFER_SIZE 31
	const int BUFFER_SIZE = 31;

	const int slot = random(PLEN2::ExternalEEPROM::SLOT_MIN(), PLEN2::ExternalEEPROM::SLOT_MAX());
	char data[_RANDOMSLOT_WRITEOVERFLOW__BUFFER_SIZE] = { 0 };


	int expected = -1;
	int actual   = ext_eeprom.writeSlot(slot, data, BUFFER_SIZE);


	assertEqual(expected, actual);
}

test(SlotOverflow_Read)
{
	#define _SLOTOVERFLOW_READ__BUFFER_SIZE 1
	const int BUFFER_SIZE = 1;

	const int slot = PLEN2::ExternalEEPROM::SLOT_MAX();
	char data[_SLOTOVERFLOW_READ__BUFFER_SIZE] = { 0 };


	int expected = -1;
	int actual   = ext_eeprom.readSlot(slot, data, BUFFER_SIZE);


	assertEqual(expected, actual);
}

test(SlotOverflow_Write)
{
	#define _SLOTOVERFLOW_WRITE__BUFFER_SIZE 1
	const int BUFFER_SIZE = 1;

	const int slot = PLEN2::ExternalEEPROM::SLOT_MAX();
	char data[_SLOTOVERFLOW_WRITE__BUFFER_SIZE] = { 0 };


	int expected = -1;
	int actual   = ext_eeprom.writeSlot(slot, data, BUFFER_SIZE);


	assertEqual(expected, actual);
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	randomSeed(analogRead(0));

	Serial.begin(115200);
	while(!Serial); // for the Arduino Leonardo/Micro only.
}

void loop()
{
	Test::run();
}
