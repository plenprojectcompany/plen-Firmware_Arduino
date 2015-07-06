/*!
	@file   Main.h
	@brief  Arduinoにおける、setup()とloop()の実装を提供します。
	@author Kazuyuki TAKASE
*/

/*!
	@mainpage

	PLEN2 - Atmega32u4向け ファームウェア
	============================================================================
	Copyright (c) 2015.
	---
	- [Kazuyuki TAKASE](https://github.com/Guvalif)
	- [PLEN Project Company Ltd.](http://plen.jp)

	Build enviroment.
	---
	- Windows 8.1 Professional edition
	- Windows 7 Home Premium
	- Arduino IDE ver.1.6.0
	- Sublime Text 2 ver.2.0.2
	- [Stino](https://github.com/Robot-Will/Stino)
	- [Arduino Unit](https://github.com/mmurdoch/arduinounit)

	License.
	---
	This software is released under the MIT License.
*/

#ifndef _PLEN2__MAIN_H_
#define _PLEN2__MAIN_H_

namespace PLEN2
{
	/*!
		@brief 初期化メソッド

		必要な初期化処理はこちらに記述してください。

		@attention
		デジタルピンの出力は初期レベルを与えない場合不定となります。
		モード設定を行うようなピンの初期化は必ず行ってください。
	*/
	void setup();
	
	/*!
		@brief メインポーリングループ

		繰り返し実行したい処理はこちらに記述してください。

		@attention
		動作を強制終了したい場合、exit()命令を使用する必要があります。
		return命令では、再度ループが実行されてしまいます。
	*/
	void loop();
}

#endif // _PLEN2__MAIN_H_