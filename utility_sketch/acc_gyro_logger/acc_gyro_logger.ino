#include "AccelerationGyroSensor.h"


namespace
{
	PLEN2::AccelerationGyroSensor acc_gyro;
}


void setup()
{
	// noop.
}

void loop()
{
	acc_gyro.dump();

	delay(100);
}
