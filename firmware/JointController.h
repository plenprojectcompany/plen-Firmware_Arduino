/*!
	@file   JointController.h
	@brief  関節の管理クラスを提供します。
	@author Kazuyuki TAKASE
*/

#ifndef _PLEN2_JOINT_CONTROLLER_H_
#define _PLEN2_JOINT_CONTROLLER_H_

namespace PLEN2
{
	/*!
		@brief 関節の管理クラス

		@note
		Atmega32u4では、ひとつのタイマでPWM信号を最大3ラインまで出力可能です。
		1ラインに8出力のマルチプレクサを接続することで、計24個までの
		サーボモータを制御することができます。
		<br>
		PLEN2で用いるサーボモータの許容最小パルス幅は0.78[msec]，
		許容最大パルス幅は2.18[msec]と、それぞれ実測値として確認されています。
		また、その際の可動範囲は130[deg]となります。
		ただし、本ファームウェアでは個体差を考慮して少なめに見積もっています。
	*/
	class JointController
	{
	private:
		/*!
			@brief 関節設定の管理オブジェクト
		*/
		struct JointSetting
		{
			unsigned int MIN;  //!< 関節可動域最小値の設定
			unsigned int MAX;  //!< 関節可動域最大値の設定
			unsigned int HOME; //!< 関節初期位置の設定
		};

		//! @brief 
		inline static const int SUM()                  { return 24;   }
		#define               __SUM                             24

		inline static const int ANGLE_MIN()            { return 300;  }
		inline static const int ANGLE_MAX()            { return 1500; }
		inline static const int ANGLE_NEUTRAL()        { return 900;  }

		inline static const int PWM_MIN()              { return 816;  }
		inline static const int PWM_MAX()              { return 492;  }
		inline static const int PWM_NEUTRAL()          { return 654;  }

		inline static const int CONFIG_FLAG_ADDRESS()  { return 0;    }
		inline static const int CONFIG_FLAG_VALUE()    { return 0;    }
		inline static const int CONFIG_BEGIN_ADDRESS() { return 1;    }

		JointSetting _SETTINGS[__SUM];

		void loadSettings();

	public:
		JointController();
	};
}

#endif // _PLEN2_JOINT_CONTROLLER_H_


void PLEN2::JointController::loadSettings()
{
	#if _DEBUG
		System::output_serial->println(F("in fuction : Joint::init()"));
	#endif

	unsigned char* filler = (unsigned char*)_SETTINGS;
	
	if (EEPROM.read(CONFIG_FLAG_ADDRESS()) != CONFIG_FLAG_VALUE())
	{
		EEPROM.write(CONFIG_FLAG_ADDRESS(), CONFIG_FLAG_VALUE());

		for (int index = 0; index < sizeof(_SETTINGS); index++)
		{
			EEPROM.write(CONFIG_BEGIN_ADDRESS() + index, filler[index]);
		}
	}
	else
	{
		for (int index = 0; index < sizeof(SETTINGS); index++)
		{
			filler[index] = EEPROM.read(CONFIG_BEGIN_ADDRESS() + index);
		}
	}
}


PLEN2::JointController::JointController()
{
	_SETTINGS[0]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 900);  // [01] 左：肩ピッチ
	_SETTINGS[1]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1150); // [02] 左：腿ヨー
	_SETTINGS[2]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1200); // [03] 左：肩ロール
	_SETTINGS[3]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 800);  // [04] 左：肘ロール
	_SETTINGS[4]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 800);  // [05] 左：腿ロール
	_SETTINGS[5]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 850);  // [06] 左：腿ピッチ
	_SETTINGS[6]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1400); // [07] 左：膝ピッチ
	_SETTINGS[7]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1200); // [08] 左：足首ピッチ
	_SETTINGS[8]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 850);  // [09] 左：足首ロール
	_SETTINGS[9]  = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[10] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[11] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[12] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 900);  // [10] 右：肩ピッチ
	_SETTINGS[13] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 950);  // [11] 右：腿ヨー
	_SETTINGS[14] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 550);  // [12] 右：肩ロール
	_SETTINGS[15] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1100); // [13] 右：肘ロール
	_SETTINGS[16] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1000); // [14] 右：腿ロール
	_SETTINGS[17] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1100); // [15] 右：腿ピッチ
	_SETTINGS[18] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 400);  // [16] 右：膝ピッチ
	_SETTINGS[19] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 580);  // [17] 右：足首ピッチ
	_SETTINGS[20] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), 1000); // [18] 右：足首ロール
	_SETTINGS[21] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[22] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
	_SETTINGS[23] = JointSetting(ANGLE_MIN(), ANGLE_MAX(), ANGLE_NEUTRAL());
}