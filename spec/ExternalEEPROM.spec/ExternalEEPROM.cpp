// Arduinoライブラリ関連
#include "Arduino.h"
#include <Wire.h>

// 独自ライブラリ関連
#include "ExternalEEPROM.h"


PLEN2::ExternalEEPROM::ExternalEEPROM()
{
	Wire.begin();
	Wire.setClock(CLOCK());
}


int PLEN2::ExternalEEPROM::readBlock(
	unsigned int slot,
	char         data[],
	unsigned int read_size
)
{
	#if _DEBUG
		System::output_serial->println(F("in fuction : ExternalEEPROM::readBlock()"));
	#endif

	if (read_size > SLOT_SIZE())
	{
		return -1;
	}

	int  slave_address = ADDRESS();
	long data_address  = (long)slot * SLOT_SIZE();

	if (data_address >= (SIZE() / 2))
	{
		slave_address |= _BV(SELECT_BIT()); // B0 = 1のメモリブロックを選択
		data_address  -= (SIZE() / 2);
	}

	#if _DEBUG_EXTEEPROM
		System::output_serial->print(F("slave_address : "));
		System::output_serial->println(slave_address, HEX);
		System::output_serial->print(F("data_address : "));
		System::output_serial->println(data_address, HEX);
	#endif

	Wire.beginTransmission(slave_address);
	Wire.write((byte)(data_address >> 8));     // High側アドレスを指定
	Wire.write((byte)(data_address & 0x00ff)); // Low側アドレスを指定
	int ret = Wire.endTransmission();

	if (ret == 0)
	{
		ret = Wire.requestFrom(slave_address, read_size);
		if (ret == read_size)
		{
			for (int index = 0; index < read_size; index++)
			{
				data[index] = Wire.read();
			}
		}
		else
		{
			ret = -1;
		}
	}
	else
	{
		ret = - 1;
	}

	return ret;
}


int PLEN2::ExternalEEPROM::writeBlock(
	unsigned int slot,
	const char   data[],
	unsigned int write_size
)
{
	#if _DEBUG
		System::output_serial->println(F("in fuction : ExternalEEPROM::writeBlock()"));
	#endif

	if (write_size > SLOT_SIZE())
	{
		return -1;
	}

	int  slave_address = ADDRESS();
	long data_address  = (long)slot * SLOT_SIZE();

	if (data_address >= (SIZE() / 2))
	{
		slave_address |= _BV(SELECT_BIT()); // B0 = 1のメモリブロックを選択
		data_address  -= (SIZE() / 2);
	}

	#if _DEBUG_EXTEEPROM
		System::output_serial->print(F("slave_address : "));
		System::output_serial->println(slave_address, HEX);
		System::output_serial->print(F("data_address : "));
		System::output_serial->println(data_address, HEX);
	#endif

	Wire.beginTransmission(slave_address);
	Wire.write((byte)(data_address >> 8));     // High側アドレスを指定
	Wire.write((byte)(data_address & 0x00ff)); // Low側アドレスを指定

	for (int index = 0; index < write_size; index++)
	{
		Wire.write(data[index]);
	}

	int ret = Wire.endTransmission();
	delay(10);

	return ret;
}