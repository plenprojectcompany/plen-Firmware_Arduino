#line 2 "AccelerationGyroSensor.spec.ino"


#include <ArduinoUnit.h>
#include "AccelerationGyroSensor.h"


#define _TEST_USER true //!< ユーザテストについても実行します。

namespace
{
	PLEN2::AccelerationGyroSensor acc_gyro;
}


/*!
	@brief 重力軸がX軸となる条件のテスト
*/
test(GravityAxisX)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();

			int acc_x = acc_gyro.getAccelerationX();
			int acc_y = acc_gyro.getAccelerationY();
			int acc_z = acc_gyro.getAccelerationZ();

			Serial.print("acc_x : ");
			Serial.println(acc_x);

			pass();
			break;

			// if (acc_x > acc_y && acc_x > acc_z)
			// {
			// 	pass();
			// }
		}
	#else
		skip();
	#endif
}


/*!
	@brief 重力軸がY軸となる条件のテスト
*/
test(GravityAxisY)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();

			int acc_x = acc_gyro.getAccelerationX();
			int acc_y = acc_gyro.getAccelerationY();
			int acc_z = acc_gyro.getAccelerationZ();

			Serial.print("acc_y : ");
			Serial.println(acc_y);

			pass();
			break;

			// if (acc_y > acc_z && acc_y > acc_x)
			// {
			// 	pass();
			// }
		}
	#else
		skip();
	#endif
}


/*!
	@brief 重力軸がZ軸となる条件のテスト
*/
test(GravityAxisZ)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();

			int acc_x = acc_gyro.getAccelerationX();
			int acc_y = acc_gyro.getAccelerationY();
			int acc_z = acc_gyro.getAccelerationZ();

			Serial.print("acc_z : ");
			Serial.println(acc_z);

			pass();
			break;

			// if (acc_z > acc_x && acc_z > acc_y)
			// {
			// 	pass();
			// }
		}
	#else
		skip();
	#endif
}


/*!
	@brief 回転軸がRoll軸となる条件のテスト
*/
test(GyroAxisRoll)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();

			int gyro_roll  = acc_gyro.getGyroRoll();
			int gyro_pitch = acc_gyro.getGyroPitch();
			int gyro_yaw   = acc_gyro.getGyroYaw();

			Serial.print("gyro_roll : ");
			Serial.println(gyro_roll);

			pass();
			break;

			// if (gyro_roll > gyro_pitch && gyro_roll > gyro_yaw)
			// {
			// 	pass();
			// }
		}
	#else
		skip();
	#endif
}


/*!
	@brief 回転軸がPitch軸となる条件のテスト
*/
test(GyroAxisPitch)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();

			int gyro_roll  = acc_gyro.getGyroRoll();
			int gyro_pitch = acc_gyro.getGyroPitch();
			int gyro_yaw   = acc_gyro.getGyroYaw();

			Serial.print("gyro_pitch : ");
			Serial.println(gyro_pitch);

			pass();
			break;

			// if (gyro_pitch > gyro_yaw && gyro_pitch > gyro_roll)
			// {
			// 	pass();
			// }
		}
	#else
		skip();
	#endif
}


/*!
	@brief 回転軸がYaw軸となる条件のテスト
*/
test(GyroAxisYaw)
{
	#if _TEST_USER
		while (true)
		{
			acc_gyro.sampling();

			int gyro_roll  = acc_gyro.getGyroRoll();
			int gyro_pitch = acc_gyro.getGyroPitch();
			int gyro_yaw   = acc_gyro.getGyroYaw();

			Serial.print("gyro_yaw : ");
			Serial.println(gyro_yaw);

			pass();
			break;

			// if (gyro_yaw > gyro_roll && gyro_yaw > gyro_pitch)
			// {
			// 	pass();
			// }
		}
	#else
		skip();
	#endif
}


/*!
	@brief アプリケーション・エントリポイント
*/
void setup()
{
	while (!Serial); // for the Arduino Leonardo/Micro only.

	Serial.println("Test started.");
	Serial.println("=============");
}

void loop()
{
	Test::run();
}
