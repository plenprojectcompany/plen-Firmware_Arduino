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
		*/
		#define _PLEN2__PURSER_COMBINATOR__BUFFER__LENGTH 32

	public:
		//! @brief バッファサイズ
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

	void m_abort();

public:
	PurserCombinator();

	void readByte(char byte);
	bool accept();
	void transition();
};

#endif // _PLEN2__PURSER_COMBINATOR_H_