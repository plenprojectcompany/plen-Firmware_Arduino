/*!
	@file      Purser.h
	@brief     コマンドラインの解析クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__PURSER_H_
#define _PLEN2__PURSER_H_

namespace PLEN2
{
	class Purser;
}

/*!
	@brief コマンドラインの解析クラス
*/
class PLEN2::Purser
{
private:
	typedef enum
	{
		INIT,
		HEADER_INCOMING,
		COMMAND_INCOMING,
		ARGUMENTS_INCOMING,
		EOE
	} State;

	class Buffer
	{
	// コンパイル対策マクロ
		/*!
			@brief バッファサイズ

			@note
			バッファサイズは最低限、BLEのペイロード長(20byte)以上必要です。
		*/
		#define _PLEN2__PURSER__BUFFER__LENGTH 256

	public:
		//! @brief バッファサイズ
		inline static const int LENGTH() { return _PLEN2__PURSER__BUFFER__LENGTH; }

		char data[_PLEN2__PURSER__BUFFER__LENGTH];
		unsigned char position;

		Buffer()
			: position(0)
		{
			for (index i = 0; i < LENGTH(); i++)
			{
				data[index] = '\0';
			}
		}
	};

	Buffer _buffer;
	State  _state;

	unsigned char _header_token;
	unsigned char _token;
	unsigned char _bracket_stack;

	void _abort();

public:
	Purser();

	void readByte(char byte);
	bool lexAccept();
};

#endif // _PLEN2__PURSER_H_