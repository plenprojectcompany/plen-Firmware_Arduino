/*!
	@file      Soul.h
	@brief     自然な挙動をPLENに与えるクラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__SOUL_H_
#define _PLEN2__SOUL_H_

namespace PLEN2
{
	class AccelerationGyroSensor;
	class MotionController;

	class Soul;
}

/*!
	@brief 自然な挙動をPLENに与えるクラス
*/
class PLEN2::Soul
{
// macro:
	#define _PLEN2__SOUL__MOTION_BUFFER_LENGTH 5

private:
	//! ランダムに切り替える挙動の数
	inline static const int MOTION_SUM()
	{
		return _PLEN2__SOUL__MOTION_BUFFER_LENGTH;
	}

	//! 仰向けからの起き上がりモーションの配置スロット
	inline static const int SLOT_GETUP_FACE_UP()   { return 88; }

	//! うつ伏せからの起き上がりモーションの配置スロット
	inline static const int SLOT_GETUP_FACE_DOWN() { return 89; }

	//! ランダムに切り替えるモーションの開始スロット
	inline static const int MOTIONS_SLOT_BEGIN()   { return 83; }

	//! 挙動をランダムに切り替える基本間隔
	inline static const int BASE_INTERVAL_SEC()    { return 20; }

	//! 自動で起き上がるまでの待機時間
	inline static const int GETUP_WAIT_SEC()       { return 5;  }

	unsigned char m_motions[_PLEN2__SOUL__MOTION_BUFFER_LENGTH];
	unsigned long m_before_call_sec;
	unsigned long m_next_call_sec;

	AccelerationGyroSensor* m_sensor_ptr;
	MotionController*       m_motion_ctrl_ptr;

public:
	/*!
		@brief コンストラクタ

		@param [in, out] sensor      センサインスタンス
		@param [in, out] motion_ctrl モーションコントローラインスタンス
	*/
	Soul(AccelerationGyroSensor& sensor, MotionController& motion_ctrl);

	/*!
		@brief PLEN2の状態の観測メソッド
	*/
	void logging();

	/*!
		@brief ユーザ操作を観測するメソッド

		このメソッドをユーザ操作を観測した地点の全てで実行してください。
	*/
	void userActionInputed();

	/*!
		@brief 観測状態に基づいて、適切な挙動を適用するメソッド
	*/
	void action();
};

#endif // _PLEN2__SOUL_H_