/*!
	@file   MotionController.h
	@brief  モーションの管理クラスを提供します。
	@author Kazuyuki TAKASE
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
private:
	JointController* _p_joint_ctrl;

	unsigned char _transition_count;
	

public:
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
		inline static const int SLOT_MIN()     { return 0;   }

		//! @brief スロット番号の最大値
		inline static const int SLOT_MAX()     { return 100; }

		//! @brief フレーム数の最小値
		inline static const int FLAMENUM_MIN() { return 1;   }

		//! @brief フレーム数の最大値
		inline static const int FLAMENUM_MAX() { return 20;  }

		unsigned char slot;                                               //!< スロット番号
		char          name[_PLEN2__MOTIONCONTROLLER__HEADER__NAMELENGTH]; //!< モーション名
		unsigned char codes[_PLEN2__MOTIONCONTROLLER__HEADER__CODESIZE];  //!< コード領域
		unsigned char frame_num;                                          //!< フレーム数
	};

	class Frame
	{
	public:
		/*!
			@brief フレーム更新間隔(ミリ秒)

			@sa
			JointController.cpp 内、ISR(TIMER1_OVF_vect)もご参照ください。
		*/
		inline static const int UPDATE_INTERVAL_MS() { return 32; }

		unsigned char number;                                    //!< フレーム番号
		unsigned int  transition_time_ms;                        //!< 遷移時間
		int           joint_angle[_PLEN2__JOINTCONTROLLER__SUM]; //!< 各関節角度
	};

	MotionController(JointController& joint_ctrl);

	void setHeader(const Header* p_header);
	void getHeader(Header* p_header);
	void setFrame(unsigned char slot, const Frame* p_frame);
	void getFrame(unsigned char slot, Frame* p_frame);

	bool playing();
	bool frameUpdatable();
	bool frameUpdateFinished();

	void play(unsigned char slot);
	void stop();
	void frameBuffering();

	void dump(unsigned char slot);
};

#endif // _PLEN2__MOTION_CONTROLLER_H_