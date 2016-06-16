#include <Arduino.h>

#include "System.h"
#include "AccelerationGyroSensor.h"


namespace
{
    PLEN2::AccelerationGyroSensor acc_gyro;

    unsigned long call_count = 1;
}


void setup()
{
    volatile PLEN2::System s;

    while (!Serial);

    delay(3000); //!< @attention A wait for starting up BLE MCU's firmware
}

void loop()
{
    PLEN2::System::debugSerial().print(acc_gyro.sampling() ? F("OK : ") : F("NG : "));
    PLEN2::System::debugSerial().println(call_count++);

    acc_gyro.dump();

    delay(1000);
}
