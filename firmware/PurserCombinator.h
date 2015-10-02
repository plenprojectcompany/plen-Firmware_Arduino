/*!
	@file      PurserCombinator.h
	@brief     PLEN2におけるコマンドラインの解析クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__PURSER_COMBINATOR_H_
#define _PLEN2__PURSER_COMBINATOR_H_

namespace PLEN2
{
	class PurserCombinator;
}

namespace Utility
{
	class AbstractPurser;
}

/*!
	@brief コマンドラインの解析クラス
*/
class PLEN2::PurserCombinator
{
// friend:
	friend bool USER_DEFINED_EVENT_HANDLER(PurserCombinator* combinator_ptr);

private:
	typedef enum
	{
		READY, // same as HEADER_INCOMING
		COMMAND_INCOMING,
		ARGUMENTS_INCOMING,
		STATE_EOE
	} State;

	class Buffer
	{
	// macro:
		/*!
			@brief バッファサイズ

			@note
			バッファサイズは最低限、BLEのペイロード長(20byte)以上必要です。
			また、高速に処理を行うため2^Nを要求します。
		*/
		#define _PLEN2__PURSER_COMBINATOR__BUFFER__LENGTH 32

	public:
		//! @brief バッファ長
		inline static const int LENGTH() { return _PLEN2__PURSER_COMBINATOR__BUFFER__LENGTH; }

		char data[_PLEN2__PURSER_COMBINATOR__BUFFER__LENGTH];
		unsigned char position;

		Buffer()
			: position(0)
		{
			for (int index = 0; index < LENGTH(); index++)
			{
				data[index] = '\0';
			}
		}
	};

	Buffer m_buffer;
	State m_state;
	unsigned char m_store_length;
	Utility::AbstractPurser* m_purser[STATE_EOE];

	/*!
		@brief パース処理を中断するメソッド
	*/
	void m_abort();

public:
	/*!
		@brief コンストラクタ
	*/
	PurserCombinator();

	/*!
		@brief 1文字を読み込み、リングバッファに格納するメソッド
	*/
	void readByte(char byte);

	/*!
		@brief パーサの内部状態も加味しながら、パースを行うメソッド

		@return 判定結果
	*/
	bool accept();

	/*!
		@brief パーサの内部状態遷移メソッド

		@note
		通常は USER_DEFINED_EVENT_HANDLER() にこのメソッドの処理も委譲します。

		@sa
		firmware.ino 内，loop() 関数も併せてご参照ください。
	*/
	void transition();
};

#endif // _PLEN2__PURSER_COMBINATOR_H_