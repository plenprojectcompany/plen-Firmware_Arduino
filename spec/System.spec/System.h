/*!
	@file   System.h
	@brief  AVR MCUにおける基本機能の管理クラスを提供します。
	@author Kazuyuki TAKASE
*/

#ifndef _PLEN2__SYSTEM_H_
#define _PLEN2__SYSTEM_H_

#include "Arduino.h"

namespace PLEN2
{
	class System;
}

/*!
	@brief AVR MCUにおける基本機能の管理クラス

	具体的には以下の機能が提供されます。
	- 割り込みのattach，detach
	- 入出力シリアルの管理・提供
*/
class PLEN2::System
{
private:
	//! @brief USBシリアルインタフェースの通信速度
	inline static const long USBSERIAL_BAUDRATE() { return 2000000L; }
	
	//! @brief BLEシリアルインタフェースの通信速度
	inline static const long BLESERIAL_BAUDRATE() { return 2000000L; }

	static Stream* _p_input_serial;  //!< 入力シリアルインスタンスのポインタ
	static Stream* _p_output_serial; //!< 出力シリアルインスタンスのポインタ

public:
	//! @brief MCU内EEPROMのサイズ
	inline static const int INTERNAL_EEPROMSIZE() { return 1024; }

	/*!
		@brief コンストラクタ
	*/
	System();

	/*!
		@brief  入力シリアルインスタンスの取得メソッド

		@return 入力シリアルインスタンスの参照

		@attention
		このメソッドにより取得される入力シリアルインスタンスは、
		実行タイミングによって異なる可能性が存在します。
		つまり、戻り値をキャッシュして使い回した際の動作は不定となります。
	*/
	Stream& inputSerial();

	/*!
		@brief  出力シリアルインスタンスの取得メソッド
		
		@return 出力シリアルインスタンスの参照
	*/
	Stream& outputSerial();

	/*!
		@brief 入力シリアルインスタンスの切り替えメソッド
	*/
	void toggleInputSerial();

	/*!
		@brief タイマ1割り込みの許可メソッド
	*/
	void timer1Attach();

	/*!
		@brief タイマ1割り込みの禁止メソッド
	*/
	void timer1Detach();
};

#endif // _PLEN2__SYSTEM_H_