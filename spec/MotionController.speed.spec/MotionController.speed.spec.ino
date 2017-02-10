#line 2 "MotionController.speed.spec.ino"


#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>

#include "System.h"
#include "ExternalEEPROM.h"
#include "JointController.h"
#include "Motion.h"
#include "MotionController.h"


namespace
{
    PLEN2::JointController  joint_ctrl;
    PLEN2::MotionController motion_ctrl(joint_ctrl);
}


/*!
    @brief The application entry point
*/
void setup()
{
    using namespace PLEN2;

    System::begin();
    ExternalEEPROM::begin();

    joint_ctrl.loadSettings();


    while (!Serial);

    System::debugSerial().print(F("# Test : "));
    System::debugSerial().println(__FILE__);
}

void loop()
{
    motion_ctrl.play(0);

    motion_ctrl.updateFrame();

    motion_ctrl.loadNextFrame();

    motion_ctrl.stop();

    while (true);
}