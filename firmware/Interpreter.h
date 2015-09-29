/*!
	@file      Interpreter.h
	@brief     コードインタプリタの管理クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__INTERPRETER_H_
#define _PLEN2__INTERPRETER_H_


namespace PLEN2
{
	class Interpreter;
	class MotionController;
}

class PLEN2::Interpreter
{
// コンパイル対策マクロ:
	#define _PLEN2__INTERPRETER__QUEUESIZE 64 //!< コードキューの大きさ

public:
	/*!
		@brief コードオブジェクト
	*/
	class Code
	{
	public:
		unsigned char slot;       //!< モーションスロットの指定
		unsigned char loop_count; //!< ループ回数の指定
	};

	//! @brief キューサイズ
	inline static const int QUEUE_SIZE() { return _PLEN2__INTERPRETER__QUEUESIZE; }


	/*!
		@brief コンストラクタ

		@param [in, out] モーションコントローラインスタンス
	*/
	Interpreter(MotionController& motion_ctrl);

	/*!
		@brief コードの実行をキューに予約するメソッド

		@param [in] code コードインスタンス

		@return 実行結果
	*/
	bool pushCode(const Code& code);

	/*!
		@brief 先頭のコードを実行するメソッド

		@return 実行結果
	*/
	bool popCode();

	/*!
		@brief 実行予約済みのコードが存在するか判定するメソッド

		@return 判定結果
	*/
	bool ready();

	/*!
		@brief インタプリタの初期化メソッド
	*/
	void reset();


private:
	Code _code_queue[_PLEN2__INTERPRETER__QUEUESIZE]; //!< コードインスタンスバッファ
	unsigned char _queue_begin; //!< キューの先頭位置
	unsigned char _queue_end;   //!< キューの最後尾
	MotionController* _p_motion_ctrl; //!< モーションコントローラインスタンスへのポインタ
};

#endif // _PLEN2__INTERPRETER_H_