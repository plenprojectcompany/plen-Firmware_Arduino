#line 2 "Profiler.speed.spec"


#include <Arduino.h>

#include "System.h"
#include "Profiler.h"


/*!
    @brief The application entry point
*/
void setup()
{
    PLEN2::System::begin();

    while (!Serial);

    PLEN2::System::debugSerial().print(F("# Test : "));
    PLEN2::System::debugSerial().println(__FILE__);
}

void loop()
{
    {
        PROFILING("Loop::outerProfiler()");

        {
            PROFILING("Loop::innerProfiler()");
        }
    }

    while (true);
}
