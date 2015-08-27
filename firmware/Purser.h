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
	class Buffer
	{
	// コンパイル対策マクロ
		/*!
			@brief バッファサイズ

			@note
			バッファサイズはBLEのペイロード長(20byte)以上ならばOKです。
		*/
		#define _PLEN2__PURSER__BUFFER__LENGTH 32

	public:
		//! @brief バッファサイズ
		inline static const int LENGTH() { return _PLEN2__PURSER__BUFFER__LENGTH; }

		char data[_PLEN2__PURSER__BUFFER__LENGTH];
		char position;

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

public:
	Purser();

	void readByte(char byte);
};

#endif // _PLEN2__PURSER_H_