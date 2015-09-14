/*!
	@file      AccelerationGyroSensor.h
	@brief     加速度・ジャイロセンサの管理クラスを提供します。
	@author    Kazuyuki TAKASE
	@copyright The MIT License - http://opensource.org/licenses/mit-license.php
*/

#ifndef _PLEN2__ACCELERATION_GYRO_SENSOR_H_
#define _PLEN2__ACCELERATION_GYRO_SENSOR_H_

namespace PLEN2
{
	class AccelerationGyroSensor;
}

/*!
	@brief 加速度・ジャイロセンサの管理クラス

	@note
	加速度・ジャイロセンサの値は、頭基板からバスを介して取得する必要があります。
*/
class PLEN2::AccelerationGyroSensor
{
// コンパイル対策マクロ
	#define _PLEN2__ACCELERATION_GYRO_SENSOR__BUFFER_LENGTH 6 //!< センサ値バッファ長

private:
	//! @brief センサ値バッファ長
	inline static const int BUFFER_LENGTH() { return _PLEN2__ACCELERATION_GYRO_SENSOR__BUFFER_LENGTH; }

	int _value[_PLEN2__ACCELERATION_GYRO_SENSOR__BUFFER_LENGTH]; //!< センサ値バッファ

public:
	/*!
		@brief センサ値のサンプリングを行うメソッド

		@note
		loop() から定期的に呼び出すことを想定しています。

		@attension
		バスを介してデータの送受信を行うため、内部で割り込みと通信待ちのロックが発生します。
		そのため、コンストラクタ内や割り込みベクタ内での使用は推奨されません。
	*/
	void sampling();

	/*!
		@brief X軸の加速度を取得するメソッド

		@return X軸の加速度

		@attension
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	int getAccelerationX();

	/*!
		@brief Y軸の加速度を取得するメソッド

		@return Y軸の加速度

		@attension
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	int getAccelerationY();

	/*!
		@brief Z軸の加速度を取得するメソッド

		@return Z軸の加速度

		@attension
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	int getAccelerationZ();

	/*!
		@brief ロール軸(X軸に関する回転軸)の角加速度を取得するメソッド

		@return ロール軸の角加速度

		@attension
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	int getGyroRoll();

	/*!
		@brief ピッチ軸(Y軸に関する回転軸)の角加速度を取得するメソッド

		@return ピッチ軸の角加速度

		@attension
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	int getGyroPitch();

	/*!
		@brief ヨー軸(Z軸に関する回転軸)の角加速度を取得するメソッド

		@return ヨー軸の角加速度

		@attension
		取得される値は、sampling() 実行時の値をキャッシュしたものです。
	*/
	int getGyroYaw();
};

#endif // _PLEN2__ACCELERATION_GYRO_SENSOR_H_