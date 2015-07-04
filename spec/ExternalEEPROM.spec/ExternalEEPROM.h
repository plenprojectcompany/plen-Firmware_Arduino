/*!
	@file   ExternalEEPROM.h
	@brief  外部EEPROMの管理クラスを提供します。
	@author Kazuyuki TAKASE
*/

#ifndef _PLEN2__EXTERNAL_EEPROM_H_
#define _PLEN2__EXTERNAL_EEPROM_H_

namespace PLEN2
{
	/*!
		@brief 外部EEPROMの管理クラス

		@note
		使用しているEEPROM，24FC1025は、一度にアクセス可能な領域サイズが128byteですが、
		ArduinoのWireライブラリのバッファサイズが32byteのため、
		性能を最大限に引き出すことはできません。
		<br>
		また注意点として、書き込みの場合アドレス指定の2byteもこの32byteに含まれます。
		つまり、1度に書き込み可能な正味の領域サイズは30byteとなります。
		
		@todo
		- I2Cのバッファサイズ拡張
	*/
	class ExternalEEPROM
	{
	private:
		//! @brief I2Cインタフェースの通信速度
		inline static const long CLOCK()      { return 400000L;  }
		
		//! @brief メモリサイズ(byte)
		inline static const long SIZE()       { return 0x20000L; }
		
		//! @brief I2Cネットワーク上での機器アドレス
		inline static const int  ADDRESS()    { return 0x50;     }
		
		//! @brief メモリチップの選択ビット
		inline static const int  SELECT_BIT() { return 2;        }

	public:
		//! @brief 外部EEPROMの1スロットの大きさ
		inline static const int SLOT_SIZE() { return 32; }

		//! @brief 外部EEPROMのスロットの最小値
		inline static const int SLOT_MIN()  { return 0;  }

		//! @brief 外部EEPROMのスロットの最大値
		inline static const int SLOT_MAX()  { return SIZE() / SLOT_SIZE(); }

		/*!
			@brief コンストラクタ
		*/
		ExternalEEPROM();

		/*!
			@brief 外部EEPROMを1スロットごとに読み込むメソッド

			@param [in]  slot      読み込みたいスロットを番号で指定します。
			@param [out] data[]    スロットから読み込んだデータを格納する配列を指定します。
			@param [in]  read_size スロットから読み込みたいデータサイズを指定します。

			@return 実行結果
			@retval 非0 成功 (通常、read_sizeと同じ値となります。)
			@retval -1  失敗
		*/
		int readSlot(unsigned int slot, char data[], unsigned int read_size);

		/*!
			@brief 外部EEPROMを1スロットごとに書き込むメソッド

			@param [in] slot       書き込みたいスロットを番号で指定します。
			@param [in] data[]     スロットへ書き込みたいデータを格納した配列を指定します。
			@param [in] write_size スロットへ書き込みたいデータサイズを指定します。

			@return 実行結果
			@retval 0  成功
			@retval -1 **write_size**がスロットサイズより大きい
			@retval 1  送ろうとしたデータが送信バッファのサイズを超えた
			@retval 2  スレーブ・アドレスを送信し、NACKを受信した
			@retval 3  データ・バイトを送信し、NACKを受信した
			@retval 4  その他のエラー

			@attention
			外部EEPROMへの書き込みには時間がかかります。(定格で3[msec]を要します。)
			本実装では、メソッドの終了時にdelay()を10[msec]挿入してあります。
		*/
		int writeSlot(unsigned int slot, const char data[], unsigned int write_size);
	};
}

#endif // _PLEN2__EXTERNAL_EEPROM_H_