/*!
	@file   JointController.h
	@brief  関節の管理クラスを提供します。
	@author Kazuyuki TAKASE
*/

#ifndef _PLEN2__JOINT_CONTROLLER_H_
#define _PLEN2__JOINT_CONTROLLER_H_

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
		ただし、本実装では個体差を考慮して少なめに見積もっています。
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

		#define _PLEN2_JOINTCONTROLLER__SUM 24 //!< 関節の実装個数
		//! @brief 関節の実装個数
		inline static const int SUM() { return _PLEN2_JOINTCONTROLLER__SUM; }

		//! @brief 関節角度の最小値
		inline static const int ANGLE_MIN()              { return 300;  }

		//! @brief 関節角度の最大値
		inline static const int ANGLE_MAX()              { return 1500; }

		//! @brief 関節角度の中間値
		inline static const int ANGLE_NEUTRAL()          { return 900;  }

		//! @brief パルス幅変調波の最小値
		inline static const int PWM_MIN()                { return 816;  }

		//! @brief パルス幅変調波の最大値
		inline static const int PWM_MAX()                { return 492;  }

		//! @brief パルス幅変調波の中間値
		inline static const int PWM_NEUTRAL()            { return 654;  }

		//! @brief 初期化フラグの保持アドレス
		inline static const int FLAG_ADDRESS()           { return 0;    }
		
		//! @brief 初期化フラグの値
		inline static const int FLAG_VALUE()             { return 0;    }
		
		//! @brief 関節設定の保持アドレス
		inline static const int SETTINGS_BEGIN_ADDRESS() { return 1;    }

		JointSetting _SETTINGS[_PLEN2_JOINTCONTROLLER__SUM];         //!< 関節設定の管理インスタンス
		JointSetting _SETTINGS_INITIAL[_PLEN2_JOINTCONTROLLER__SUM]; //!< 関節設定初期値の管理インスタンス

		/*!
			@brief 関節設定の読み込みメソッド
		*/
		void loadSettings();

	public:
		/*!
			@brief タイマ1 オーバーフロー割り込みベクタ，呼び出し回数

			@attention
			本来はprivateにされるべき変数です。タイマ1 オーバーフロー割り込みベクタから
			参照するためにpublicにされているので、基本的に外部からはこの変数を参照しないでください。
		*/
		volatile static unsigned char _overflow_count;

		/*!
			@brief パルス幅変調波，出力用配列

			@attention
			本来はprivateにされるべき変数です。タイマ1 オーバーフロー割り込みベクタから
			参照するためにpublicにされているので、基本的に外部からはこの変数を参照しないでください。
		*/
		volatile static unsigned int _pwms[_PLEN2_JOINTCONTROLLER__SUM];

		/*!
			@brief コンストラクタ
		*/
		JointController();

		/*!
			@brief 
		*/
		bool setMinAngle(unsigned char joint_id, unsigned int angle);

		/*!
			@brief 
		*/
		bool setMaxAngle(unsigned char joint_id, unsigned int angle);

		/*!
			@brief 
		*/
		bool setNeutralAngle(unsigned char joint_id, unsigned int angle);

		/*!
			@brief 
		*/
		bool setAngle(unsigned char joint_id, unsigned int angle);

		/*!
			@brief 関節設定のダンプコマンド
		*/
		void dump();
	};
}

#endif // _PLEN2__JOINT_CONTROLLER_H_