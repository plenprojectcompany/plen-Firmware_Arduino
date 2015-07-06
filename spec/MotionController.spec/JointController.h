/*!
	@file   JointController.h
	@brief  関節の管理クラスを提供します。
	@author Kazuyuki TAKASE
*/

#ifndef _PLEN2__JOINT_CONTROLLER_H_
#define _PLEN2__JOINT_CONTROLLER_H_

namespace PLEN2
{
	class JointController;
}

/*!
	@brief 関節の管理クラス

	@note
	Atmega32u4では、ひとつのタイマでPWM信号を最大3ラインまで出力可能です。
	1ラインに8出力のマルチプレクサを接続することで、計24個までのサーボモータを制御可能です。
	<br><br>
	PLEN2で用いるサーボモータの許容最小パルス幅は0.78[msec]，許容最大パルス幅は2.18[msec]と、
	それぞれ実測値として確認されています。また、その際の可動範囲は130[deg]となります。
	ただし、本実装では個体差を考慮して少なめに見積もっています。
*/
class PLEN2::JointController
{
// コンパイル対策マクロ:
	#define _PLEN2__JOINTCONTROLLER__SUM 24 //!< 関節の実装個数

private:
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

	/*!
		@brief 関節設定の管理クラス
	*/
	class JointSetting
	{
	public:
		unsigned int MIN;  //!< 関節角度最小値の設定
		unsigned int MAX;  //!< 関節角度最大値の設定
		unsigned int HOME; //!< 関節角度初期値の設定

		/*!
			@brief コンストラクタ
		*/
		JointSetting()
			: MIN(ANGLE_MIN()), MAX(ANGLE_MAX()), HOME(ANGLE_NEUTRAL())
		{
			// noop.
		}

		/*!
			@brief コンストラクタ

			@param [in] min  関節角度最小値
			@param [in] max  関節角度最大値
			@param [in] home 関節角度初期値
		*/
		JointSetting(unsigned int min, unsigned int max, unsigned int home)
			: MIN(min), MAX(max), HOME(home)
		{
			// noop.
		}
	};

	JointSetting _SETTINGS_INITIAL[_PLEN2__JOINTCONTROLLER__SUM]; //!< 関節設定初期値の管理インスタンス
	JointSetting _SETTINGS[_PLEN2__JOINTCONTROLLER__SUM];         //!< 関節設定の管理インスタンス

public:
	class Multiplexer {
	public:
		//! @brief マルチプレクサの実装個数
		inline static const int SUM()            { return 3; }
		
		//! @brief 1つのマルチプレクサで制御可能な信号線数
		inline static const int SELECTABLE_NUM() { return 8; }
	};

	//! @brief 関節の実装個数
	inline static const int SUM() { return _PLEN2__JOINTCONTROLLER__SUM; }

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
	static unsigned int _pwms[_PLEN2__JOINTCONTROLLER__SUM];

	/*!
		@brief コンストラクタ
	*/
	JointController();

	/*!
		@brief 関節設定の読み込みメソッド

		Atmega32u4の内部EEPROMから、関節設定を読み出します。
		内部EEPROM内に値が存在しない場合は、デフォルトの値を書き込みます。

		@attention
		本来はコンストラクタ内で呼び出すべき処理ですが、
		AVR MCUではコンストラクタ内で割り込みが発生した際、プログラムが強制的に停止するようです。
		<br><br>
		本メソッドは、内部でシリアル通信と内部EEPROMへの読み書きを行うため、
		その際に割り込みが発生します。そのため、コンストラクタ内には本メソッドを記述できません。
	*/
	void loadSettings();

	/*!
		@brief 関節設定の初期化メソッド

		内部EEPROM内に、デフォルトの関節設定を書き込みます。
	*/
	void resetSettings();

	/*!
		@brief 指定した関節の角度最小値を取得するメソッド

		@param [in] joint_id 角度最小値を取得したい関節を番号で指定します。

		@return 関節角度最小値
		@retval -1 **joint_id**が不正
	*/
	int getMinAngle(unsigned char joint_id);
	
	/*!
		@brief 指定した関節の角度最大値を取得するメソッド

		@param [in] joint_id 角度最大値を取得したい関節を番号で指定します。

		@return 関節角度最大値
		@retval -1 **joint_id**が不正
	*/
	int getMaxAngle(unsigned char joint_id);
	
	/*!
		@brief 指定した関節の角度初期値を取得するメソッド

		@param [in] joint_id 角度初期値を取得したい関節を番号で指定します。

		@return 関節角度初期値
		@retval -1 **joint_id**が不正
	*/
	int getHomeAngle(unsigned char joint_id);

	/*!
		@brief 指定した関節の角度最小値を、指定した値に設定するメソッド

		@param [in] joint_id 角度最小値を設定したい関節を番号で指定します。
		@param [in] angle    角度を分解能1/10°単位で指定します。

		@retun 実行結果
	*/
	bool setMinAngle(unsigned char joint_id, unsigned int angle);

	/*!
		@brief 指定した関節の角度最大値を、指定した値に設定するメソッド

		@param [in] joint_id 角度最大値を設定したい関節を番号で指定します。
		@param [in] angle    角度を分解能1/10°単位で指定します。

		@retun 実行結果
	*/
	bool setMaxAngle(unsigned char joint_id, unsigned int angle);

	/*!
		@brief 指定した関節の角度初期値を、指定した値に設定するメソッド

		@param [in] joint_id 角度初期値を設定したい関節を番号で指定します。
		@param [in] angle    角度を分解能1/10°単位で指定します。

		@retun 実行結果
	*/
	bool setHomeAngle(unsigned char joint_id, unsigned int angle);

	/*!
		@brief 指定した関節の角度を、指定した値に設定するメソッド

		@param [in] joint_id 角度を設定したい関節を番号で指定します。
		@param [in] angle    角度を分解能1/10°単位で指定します。

		@retun 実行結果

		@attention
		必ずしも、**angle**の値がそのまま設定されるわけではありません。
		ユーザが指定した関節角度の最大・最小値、およびサーボモータ自体の可動範囲内に丸めた後、
		その値を設定します。単体テストの際にはそれを考慮して、テストを記述する必要があります。
	*/
	bool setAngle(unsigned char joint_id, unsigned int angle);

	/*!
		@brief 関節設定のダンプコマンド
	*/
	void dump();
};

#endif // _PLEN2__JOINT_CONTROLLER_H_
