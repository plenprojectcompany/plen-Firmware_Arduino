/*!
	@file      MotionController.h
	@brief     モーションの管理クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__MOTION_CONTROLLER_H_
#define _PLEN2__MOTION_CONTROLLER_H_

#include "JointController.h"

namespace PLEN2
{
	class MotionController;
}

/*!
	@brief モーションの管理クラス
*/
class PLEN2::MotionController
{
public:
	/*!
		@brief モーションヘッダ構造体

		@attention
		本ファームウェアではインスタンスのメモリ配置をそのままEEPROMへバックアップしています。
		そのため、メンバの宣言順序を変更した場合、モーションを全てインストールし直さなければ、
		PLENは正常に動作しなくなります。
	*/
	class Header
	{
	// コンパイル対策マクロ:
		#define _PLEN2__MOTIONCONTROLLER__HEADER__NAMELENGTH 20 //!< モーション名の長さ
		#define _PLEN2__MOTIONCONTROLLER__HEADER__CODESIZE   3  //!< コード領域の大きさ

	public:
		//! @brief モーション名の長さ
		inline static const int NAME_LENGTH()  { return _PLEN2__MOTIONCONTROLLER__HEADER__NAMELENGTH; }

		//! @brief コード領域の大きさ
		inline static const int CODE_SIZE()    { return _PLEN2__MOTIONCONTROLLER__HEADER__CODESIZE;   }

		//! @brief スロット番号の最小値
		inline static const int SLOT_MIN()     { return 0;  }

		//! @brief スロット番号の最大値
		inline static const int SLOT_MAX()     { return 99; }

		//! @brief フレーム数の最小値
		inline static const int FRAMENUM_MIN() { return 1;  }

		//! @brief フレーム数の最大値
		inline static const int FRAMENUM_MAX() { return 20; }

		char          name[_PLEN2__MOTIONCONTROLLER__HEADER__NAMELENGTH]; //!< モーション名
		unsigned char codes[_PLEN2__MOTIONCONTROLLER__HEADER__CODESIZE];  //!< コード領域
		unsigned char frame_num;                                          //!< フレーム数
		unsigned char slot;                                               //!< スロット番号
	};

	/*!
		@brief モーションフレーム構造体

		@attention
		本ファームウェアではインスタンスのメモリ配置をそのままEEPROMへバックアップしています。
		そのため、メンバの宣言順序を変更した場合、モーションを全てインストールし直さなければ、
		PLENは正常に動作しなくなります。
	*/
	class Frame
	{
	public:
		/*!
			@brief フレーム更新間隔(ミリ秒)

			@sa
			JointController.cpp 内、ISR(TIMER1_OVF_vect)もご参照ください。
		*/
		inline static const int UPDATE_INTERVAL_MS() { return 32; }

		unsigned int  transition_time_ms;                        //!< 遷移時間
		int           joint_angle[_PLEN2__JOINTCONTROLLER__SUM]; //!< 各関節角度
		unsigned char number;                                    //!< フレーム番号
	};

	/*!
		@brief コンストラクタ

		@param [in] joint_ctrl 関節管理インスタンス
	*/
	MotionController(JointController& joint_ctrl);

	/*!
		@brief モーションヘッダを外部EEPROMへ書き込むメソッド

		@param [in] p_header モーションヘッダインスタンスのポインタ

		@return 実行結果
	*/
	bool setHeader(const Header* p_header);

	/*!
		@brief モーションヘッダを外部EEPROMから読み込むメソッド

		@param [in, out] p_header モーションヘッダインスタンスのポインタ

		@return 実行結果
	*/
	bool getHeader(Header* p_header);

	/*!
		@brief モーションフレームを外部EEPROMへ書き込むメソッド

		@param [in] slot    モーション番号
		@param [in] p_frame モーションフレームインスタンスのポインタ

		@return 実行結果
	*/
	bool setFrame(unsigned char slot, const Frame* p_frame);

	/*!
		@brief モーションフレームを外部EEPROMから読み込むメソッド

		@param [in]      slot    モーション番号
		@param [in, out] p_frame モーションフレームインスタンスのポインタ

		@return 実行結果
	*/
	bool getFrame(unsigned char slot, Frame* p_frame);

	/*!
		@brief モーションが再生中かを判定するメソッド

		@return 判定結果
	*/
	bool playing();

	/*!
		@brief モーションフレームが更新可能かを判定するメソッド

		@return 判定結果
	*/
	bool frameUpdatable();

	/*!
		@brief モーションフレームの更新が終了したかを判定するメソッド

		@return 判定結果
	*/
	bool frameUpdateFinished();

	/*!
		@brief 次に読み込み可能なモーションフレームが存在するか判定するメソッド

		@return 判定結果
	*/
	bool nextFrameLoadable();

	/*!
		@brief モーションを再生するメソッド

		@param [in] slot モーション番号
	*/
	void play(unsigned char slot);

	/*!
		@brief モーションの再生停止を予約するメソッド

		@note
		実行直後にモーションの再生が中断されるわけではなく、
		次のストップフラグをもつフレームに遭遇した段階で再生を停止します。
	*/
	void stopping();

	/*!
		@brief モーションの再生を停止するメソッド
	*/
	void stop();

	/*
		@brief 遷移中のモーションフレームを更新するメソッド
	*/
	void frameUpdate();

	/*!
		@brief 次のモーションフレームを読み込むメソッド
	*/
	void loadNextFrame();

	/*!
		@brief モーションのダンプをJSON形式で行うメソッド

		@param [in] slot モーション番号
	*/
	void dump(unsigned char slot);

// コンパイル対策マクロ
	#define _PLEN2__MOTION_CONTROLLER__FRAMEBUFFER_LENGTH 2 //!< フレームバッファ長

private:
	//! @brief フレームバッファ長
	inline static const int FRAMEBUFFER_LENGTH() { return _PLEN2__MOTION_CONTROLLER__FRAMEBUFFER_LENGTH; }

	/*!
		@brief モーションフレームのバッファリングを行うメソッド
	*/
	void frameBuffering();


	JointController* _p_joint_ctrl;  //!< 関節管理インスタンスのポインタ
	
	unsigned char _transition_count; //!< 遷移回数
	bool          _playing;          //!< モーションを再生中であるか否かの論理

	Header _header; //!< ヘッダインスタンス
	Frame  _buffer[_PLEN2__MOTION_CONTROLLER__FRAMEBUFFER_LENGTH]; //!< フレームインスタンスバッファ
	Frame* _p_frame_now;  //!< 現在フレームへのポインタ
	Frame* _p_frame_next; //!< 次点フレームへのポインタ

	long _now_fixed_points[_PLEN2__JOINTCONTROLLER__SUM];  //!< 現在値計算用固定小数点バッファ
	long _diff_fixed_points[_PLEN2__JOINTCONTROLLER__SUM]; //!< 差分計算用固定小数点バッファ
};

#endif // _PLEN2__MOTION_CONTROLLER_H_