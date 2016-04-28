#include <Arduino.h>

#include "Pin.h"
#include "System.h"


void setup()
{
    volatile PLEN2::System s;
}

void loop()
{
    using namespace PLEN2;

    if (System::USBSerial().available())
    {
        digitalWrite(Pin::RS485_TXD(), HIGH);

        System::BLESerial().write(System::USBSerial().read());
        System::BLESerial().flush();

        digitalWrite(Pin::RS485_TXD(), LOW);
    }

    if (System::BLESerial().available())
    {
        System::USBSerial().write(System::BLESerial().read());
    }
}