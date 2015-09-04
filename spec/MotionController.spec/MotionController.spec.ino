#line 2 "MotionController.spec.ino"


// Arduinoライブラリ関連
#include <Wire.h>
#include <EEPROM.h>

// 独自ライブラリ関連
#include <ArduinoUnit.h>
#include "JointController.h"
#include "MotionController.h"


namespace {
	PLEN2::JointController  joint_ctrl;
	PLEN2::MotionController motion_ctrl(joint_ctrl);
}


void setup()
{
	joint_ctrl.loadSettings();
	joint_ctrl.resetSettings();
}


void loop()
{
	// noop.
}
